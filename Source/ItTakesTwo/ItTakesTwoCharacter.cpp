// Copyright Epic Games, Inc. All Rights Reserved.

#include "ItTakesTwoCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Anim/ItTakesTwoPlayerAnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Component/GrabInterActionComponent.h"
#include "ItTakesTwo/Public/Character/CustomCharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AItTakesTwoCharacter


void AItTakesTwoCharacter::OnClimbableWallDetectionOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                           bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag(TEXT("Climbable")))
	{
		if (PickUpItem != EHandItemType::None)
		{
			return;
		}

		if (ClimbUpMonta == nullptr)
		{
			return;
		}

		if (AnimInst->GetCurrentActiveMontage() == ClimbUpMonta)
		{
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("벽에 붙음"));

		FHitResult Hit;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), OtherActor->GetActorLocation(),
		                                         ECC_Visibility, Params))
		{
			WallNormal = Hit.ImpactNormal;
			FVector LookDir = -WallNormal;
			SetActorRotation(LookDir.Rotation());
		}

		// FVector OppositeVector = OtherActor->GetActorForwardVector() * -1;
		// SetActorRotation(OppositeVector.Rotation());

		WallNormal = SweepResult.ImpactNormal;
		CurrentMovementModeState |= EMovementState::Climbing;
		SetBaseMovementMappingContext();
	}
}

void AItTakesTwoCharacter::OnClimbableWallDetectionEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("벽에서 떨어짐"));

	if (ClimbUpMonta)
	{
		if (AnimInst->GetCurrentActiveMontage() == ClimbUpMonta)
		{
			return;
		}
	}

	CurrentMovementModeState &= ~EMovementState::Climbing;
	SetBaseMovementMappingContext();
}

void AItTakesTwoCharacter::SetAnimLayer(EHandItemType Type) const
{
	if (AnimInst != nullptr)
	{
		AnimInst->OnLinkAnimClassLayers(Type);
	}
}

void AItTakesTwoCharacter::ClimbUpEnded()
{
	CurrentMovementModeState &= ~EMovementState::Climbing;
	SetBaseMovementMappingContext();
}

void AItTakesTwoCharacter::OnIgnoreInputMontaEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIgnoreMoveInput = false;
	bIgnoreInteractionInput = false;

	GetController()->SetIgnoreMoveInput(false);
	UE_LOG(LogTemp, Warning, TEXT("몽타주 재생 종료됨!"));

	if (bInterrupted)
	{
		if (Montage == ClimbUpMonta)
		{
			ClimbUpEnded();
		}
	}
}

AItTakesTwoCharacter::AItTakesTwoCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomCharacterMovementComponent>(
		ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// 커스텀 컴포넌트 부착
	GrabInterActionComponent = CreateDefaultSubobject<UGrabInterActionComponent>(TEXT("GrabInterAction"));

	// 커스텀 무브먼트 컴포넌트 캐스팅해서 들고있기
	CustomCharacterMovementComp = Cast<UCustomCharacterMovementComponent>(GetCharacterMovement());

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

int32 AItTakesTwoCharacter::GetCurrentMovementStateFlag()
{
	return static_cast<int32>(CurrentMovementModeState);
}

FVector AItTakesTwoCharacter::GetLocalVelocity() const
{
	FRotator YawRotator = FRotator(0, GetActorRotation().Yaw, 0);

	FVector LocalVelocity = YawRotator.UnrotateVector(GetVelocity());
	return LocalVelocity;
}

void AItTakesTwoCharacter::BeginPlay()
{
	// Call the base class
	Super::BeginPlay();

	AnimInst = Cast<UItTakesTwoPlayerAnimInstance>(GetMesh()->GetAnimInstance());

	if (AnimInst == nullptr)
	{
		// 1. 에디터 로그 창과 화면에 빨간색 경고 메시지 출력
		UE_LOG(LogTemp, Error, TEXT("CRITICAL ERROR: AnimInst 할당 실패! 게임을 종료합니다."));

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AnimInst is NULL! Check Output Log."));
		}

		// 1. 에디터 로그 창과 화면에 빨간색 경고 메시지 출력
		UE_LOG(LogTemp, Error, TEXT("CRITICAL ERROR: AnimInst 할당 실패! 게임을 종료합니다."));

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AnimInst is NULL! Check Output Log."));
		}

		// 2. 현재 실행 중인 게임(PIE) 세션만 종료 (에디터는 유지)if (AnimInst == nullptr)
		UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit,
		                               false);
		return;
	}

	SetAnimLayer(EHandItemType::None);


	if (GetCapsuleComponent() != nullptr)
	{
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(
			this, &AItTakesTwoCharacter::OnClimbableWallDetectionOverlap);
		GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(
			this, &AItTakesTwoCharacter::OnClimbableWallDetectionEnd);
	}

	if (ClimbUpMonta != nullptr)
	{
		ClimbUpMontageEndedDelegate.BindUObject(this, &AItTakesTwoCharacter::OnIgnoreInputMontaEnded);
	}

	UGrabInterActionComponent* GrabComp = FindComponentByClass<UGrabInterActionComponent>();
	if (GrabComp != nullptr)
	{
		//아이템 들기
		GrabComp->OnItemPickUp.BindUObject(this, &AItTakesTwoCharacter::SetPickUpItemType);
		GrabComp->OnItemPutDown.BindUObject(this, &AItTakesTwoCharacter::SetPutDownItemType);

		//버튼
		GrabComp->OnToggleSwitched.BindUObject(this, &AItTakesTwoCharacter::ToggleSwitched);
		GrabComp->OnButtonHold.BindUObject(this, &AItTakesTwoCharacter::ButtonHold);
		GrabComp->OnButtonRelease.BindUObject(this, &AItTakesTwoCharacter::ButtonRelease);

		//레버
		GrabComp->OnLeverSwitched.BindUObject(this, &AItTakesTwoCharacter::LeverActive);

		//밀고 당기기
		GrabComp->OnObjectPull.BindUObject(this, &AItTakesTwoCharacter::ObjectPull);
		GrabComp->OnPullReleased.BindUObject(this, &AItTakesTwoCharacter::PullReleased);
	}
}

bool AItTakesTwoCharacter::TryGroundPoundBreak(const FHitResult& Hit)
{
	if (CurrentGroundPoundCount >= MaxGroundPoundCount)
	{
		return false;
	}

	UAnimMontage* CurActiveMontage = AnimInst->GetCurrentActiveMontage();
	if (FName("GroundPound_Falling") == AnimInst->Montage_GetCurrentSection(GroundPoundMontage))
	{
		if (Hit.GetActor() && Hit.GetActor()->ActorHasTag("Breakable"))
		{
			Hit.GetActor()->Destroy();
			return true;
		}
	}

	return false;
}

void AItTakesTwoCharacter::TryClimbUp()
{
	float CharacterRadius, CharacterHalfHeight;

	GetCapsuleComponent()->GetScaledCapsuleSize(CharacterRadius, CharacterHalfHeight);
	FVector ForwardCheckStart = GetActorLocation() + (GetActorUpVector() * CharacterHalfHeight - 10);

	FVector ForwardCheckEnd = ForwardCheckStart + (GetActorForwardVector() * CharacterRadius * 2);

	UE_LOG(LogTemp, Warning, TEXT("오르기 시작 거리 : %f"), ForwardCheckEnd.Y - ForwardCheckStart.Y);


	FHitResult ForwardHit;
	FCollisionQueryParams Params;

	if (false == GetWorld()->LineTraceSingleByChannel(ForwardHit, ForwardCheckStart, ForwardCheckEnd, ECC_Visibility,
	                                                  Params))
	{
		DrawDebugLine(GetWorld(), ForwardCheckStart, ForwardCheckEnd, FColor::Blue, false, 10.0f);

		FHitResult DownHit;

		FVector HeightCheckStart = ForwardCheckEnd - (GetActorUpVector() * 10);
		FVector HeightCheckEnd = ForwardCheckEnd + (GetActorUpVector() * CharacterRadius * 2);

		UE_LOG(LogTemp, Warning, TEXT("오르기 여유 높이 : %f"), HeightCheckEnd.Y - HeightCheckStart.Y);


		if (false == GetWorld()->LineTraceSingleByChannel(DownHit, HeightCheckStart, HeightCheckEnd, ECC_Visibility,
		                                                  Params))
		{
			DrawDebugLine(GetWorld(), HeightCheckStart, HeightCheckEnd, FColor::Green, false, 10.0f);

			UE_LOG(LogTemp, Warning, TEXT("올라가기 동작 실행 가능"));
			if (ClimbUpMonta)
			{
				GetController()->SetIgnoreMoveInput(true);
				bIgnoreMoveInput = true;

				InputMovementVector = FVector2D::ZeroVector;

				AnimInst->Montage_Play(ClimbUpMonta);
				AnimInst->Montage_SetEndDelegate(ClimbUpMontageEndedDelegate, ClimbUpMonta);

				// MontageEndedDelegate->BindUFunction(); // 블루프린트에서 만든 함수를 쓸 때 사용. 문자열 기반으로 찾는거라 느림.
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Can't Find ClimbUp Monta"));
			}
		}
	}
}

void AItTakesTwoCharacter::SetIgnoreInputPlayingMontage(UAnimMontage* TargetMontage)
{
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AItTakesTwoCharacter::OnIgnoreInputMontaEnded);
	AnimInst->Montage_SetEndDelegate(EndDelegate);

	bIgnoreMoveInput = true;
	bIgnoreInteractionInput = true;
	GetController()->SetIgnoreMoveInput(true);
}

bool AItTakesTwoCharacter::CheckLineTrace(FVector StartVec, FVector EndVec)
{
	FHitResult HitResult;

	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this); // 자신은 충돌에서 제외

	// 라인 트레이스 수행 (ECC_Visibility 채널 사용)
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartVec,
		EndVec,
		ECC_Visibility,
		TraceParams
	);

	// 디버그용: 부딪힌 대상의 이름을 출력
	if (bHit && HitResult.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());
		UE_LOG(LogTemp, Warning, TEXT("Hit Component: %s"), *HitResult.GetComponent()->GetName());
	}

	// 시각적 확인 (5초 동안 빨간 선 표시, 충돌 지점은 점으로 표시)
	DrawDebugLine(GetWorld(), StartVec, EndVec, FColor::Green, false, 5.f, 0, 1.f);
	if (bHit)
	{
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.f, 12, FColor::Red, false, 5.f);
	}


	return HitResult.bBlockingHit;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AItTakesTwoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			PlayerController->GetLocalPlayer());
		if (EnhancedInputSubsystem != nullptr)
		{
			EnhancedInputSubsystem->AddMappingContext(DefaultMappingContext, MoveMappingPriority);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AItTakesTwoCharacter::CustomJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this,
		                                   &AItTakesTwoCharacter::CustomStopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AItTakesTwoCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AItTakesTwoCharacter::StopMove);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AItTakesTwoCharacter::Look);

		// Dash
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &AItTakesTwoCharacter::Dash);

		// Climbing
		EnhancedInputComponent->BindAction(ClimbingAction, ETriggerEvent::Triggered, this,
		                                   &AItTakesTwoCharacter::Climb);
		EnhancedInputComponent->BindAction(ClimbingAction, ETriggerEvent::Completed, this,
		                                   &AItTakesTwoCharacter::StopMove);

		// InterAction
		EnhancedInputComponent->BindAction(InterAction, ETriggerEvent::Triggered, this,
		                                   &AItTakesTwoCharacter::CustomInterAction);

		// Crouch
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this,
		                                   &AItTakesTwoCharacter::CustomCrouch);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}

void AItTakesTwoCharacter::SetLockOnMode(bool bLockOn)
{
	bIsLockOnMode = bLockOn;

	UAnimInstance* LinkedAnimInst = AnimInst->GetLinkedAnimGraphInstanceByTag(FName("ItTakesTwoPlayer"));
	UE_LOG(LogTemp, Warning, TEXT("찾기"));

	if (LinkedAnimInst != nullptr)
	{
		if (bIsLockOnMode)
		{
			UE_LOG(LogTemp, Warning, TEXT("록온 ㅇㅇ"));

			GetCharacterMovement()->bOrientRotationToMovement = false;
			LinkedAnimInst->SetRootMotionMode(ERootMotionMode::Type::RootMotionFromEverything);
		}
		else
		{
			GetCharacterMovement()->bOrientRotationToMovement = true;
			LinkedAnimInst->SetRootMotionMode(ERootMotionMode::Type::RootMotionFromMontagesOnly);
		}
	}
}

void AItTakesTwoCharacter::SetPickUpItemType(EHandItemType Type, UAnimMontage* PickUpMontage)
{
	UE_LOG(LogTemp, Log, TEXT("SetPickUpItemType %s"), *UEnum::GetValueAsString(Type));

	PickUpItem = Type;

	FName SectionName = TEXT("");

	switch (PickUpItem)
	{
	case EHandItemType::Heavy:
		SectionName = TEXT("Heavy");
		break;
	case EHandItemType::Small:
		SectionName = TEXT("Small");
		break;
	}

	AnimInst->Montage_Play(PickUpMontage);
	AnimInst->Montage_JumpToSection(SectionName, PickUpMontage);
	SetIgnoreInputPlayingMontage(PickUpMontage);
	SetAnimLayer(Type);
}

void AItTakesTwoCharacter::SetPutDownItemType(EHandItemType Type, UAnimMontage* PutDownMontage)
{
	UE_LOG(LogTemp, Log, TEXT("SetPutDownItemType %s"), *UEnum::GetValueAsString(Type));


	FName SectionName = TEXT("");

	//앞쪽에 공간이 있는지 체크
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	AActor* EquipedItem = GrabInterActionComponent->GetEquipedItem();
	if (EquipedItem)
	{
		Params.AddIgnoredActor(EquipedItem);
	}

	FVector OffSet = GetActorForwardVector() * 200.0f;
	OffSet.Z -= (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 5.0f);

	FVector TargetLocation = GetActorLocation() + OffSet;

	DrawDebugLine(GetWorld(), GetActorLocation(), TargetLocation, FColor::Red, false, 10.0f);

	// 공간이 있다면 던지고, 없다면 제자리에 내려둠.
	// 가지고 있는 아이템이 무거운지, 가볍다면 달리고 있는지 (달릴땐 한손) 판단해서 점프할 섹션의 이름 지정
	if (GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), TargetLocation, ECC_Visibility, Params))
	{
		if (Type == EHandItemType::Heavy) { SectionName = TEXT("Heavy_Throw"); }
		else if (Type == EHandItemType::Small
			&& EnumHasAnyFlags(CurrentMovementModeState, EMovementState::Sprint))
		{
			SectionName = TEXT("Small_Throw_OneHand");
		}
		else if (Type == EHandItemType::Small) { SectionName = TEXT("Small_Throw"); }
	}
	else
	{
		if (Type == EHandItemType::Heavy) { SectionName = TEXT("Heavy_InPlace"); }
		else if (Type == EHandItemType::Small
			&& EnumHasAnyFlags(CurrentMovementModeState, EMovementState::Sprint))
		{
			SectionName = TEXT("Small_InPlace_OneHand");
		}
		else if (Type == EHandItemType::Small) { SectionName = TEXT("Small_InPlace"); }
	}

	if (Hit.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor : %s"), *Hit.GetActor()->GetName());
	}

	//내려놓기 재생
	AnimInst->Montage_Play(PutDownMontage);
	AnimInst->Montage_JumpToSection(SectionName);

	SetIgnoreInputPlayingMontage(PutDownMontage);

	// //재생이 끝나면 그랩 컴포넌트의 때어내기 함수 실행
	// 노티파이로 대체함.
	// FOnMontageEnded EndDelegate;
	// EndDelegate.BindUObject(GrabInterActionComponent, &UGrabInterActionComponent::PutDownMantageEnd);
	// AnimInst->Montage_SetEndDelegate(EndDelegate);


	PickUpItem = EHandItemType::None;
	SetAnimLayer(PickUpItem);
}

void AItTakesTwoCharacter::ToggleSwitched(UAnimMontage* HitButtonMontage)
{
	if (bIgnoreMoveInput)
	{
		return;
	}

	if (HitButtonMontage == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("매개변수로 온 HitButtonMontage가 없음"));
		return;
	}

	// 랜덤하게 버튼 누르는 애니메이션 재생
	TArray<FName> SectionNames = {TEXT("Default"), TEXT("var2")};
	int32 RandomIndex = FMath::RandRange(0, SectionNames.Num() - 1);

	AnimInst->Montage_Play(HitButtonMontage);
	AnimInst->Montage_JumpToSection(SectionNames[RandomIndex], HitButtonMontage);

	SetIgnoreInputPlayingMontage(HitButtonMontage);
}

void AItTakesTwoCharacter::ButtonHold(UAnimMontage* HoldButtonMontage)
{
	UE_LOG(LogTemp, Log, TEXT("버튼 홀드 가즈아"));

	PickUpItem = EHandItemType::HoldButton;

	if (AnimInst && HoldButtonMontage)
	{
		AnimInst->Montage_Play(HoldButtonMontage);
		SetIgnoreInputPlayingMontage(HoldButtonMontage);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AnimInst가 없거나 ButtonHold 매개변수 HoldButtonMontage 몽타주 없음"));
	}
}

void AItTakesTwoCharacter::ButtonRelease(UAnimMontage* HoldButtonMontage)
{
	UE_LOG(LogTemp, Log, TEXT("버튼 홀드 끛!"));

	PickUpItem = EHandItemType::None;

	if (HoldButtonMontage == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ButtonRelease의 매개변수 HoldButtonMontage 몽타주 없음"));
		return;
	}

	if (AnimInst->Montage_IsPlaying(HoldButtonMontage))
	{
		AnimInst->Montage_JumpToSection(TEXT("HoldButton_Exit"), HoldButtonMontage);
	}
	else
	{
		AnimInst->Montage_Play(HoldButtonMontage);
		AnimInst->Montage_JumpToSection(TEXT("HoldButton_Exit"), HoldButtonMontage);
	}
}

void AItTakesTwoCharacter::LeverActive(UAnimMontage* HitLeverMontage, bool bIsLeft)
{
	UE_LOG(LogTemp, Log, TEXT("레버 상호작용 성공. 몽타주 재생"));

	AnimInst->Montage_Play(HitLeverMontage);

	if (bIgnoreMoveInput)
	{
		return;
	}

	if (HitLeverMontage == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("LeverActive 함수 매개변수로 온 HitLeverMontage가 없음"));
		return;
	}


	AnimInst->Montage_Play(HitLeverMontage);

	//오른쪽에 있으면 왼쪽에서 오른쪽으로 치는 모션?
	if (bIsLeft == false)
	{
		AnimInst->Montage_JumpToSection(TEXT("ToggleLever_Left"), HitLeverMontage);
	}
	SetIgnoreInputPlayingMontage(HitLeverMontage);
}

void AItTakesTwoCharacter::ObjectPull(UAnimMontage* PullPushMontage)
{
	UE_LOG(LogTemp, Log, TEXT("풀 푸쉬 가즈아!"));

	PickUpItem = EHandItemType::PullableObject;

	AnimInst->Montage_Play(PullPushMontage);
}

void AItTakesTwoCharacter::PullReleased(UAnimMontage* PullPushMontage)
{
	UE_LOG(LogTemp, Log, TEXT("풀 푸쉬 끛!"));

	PickUpItem = EHandItemType::None;

	AnimInst->Montage_Play(PullPushMontage);
	AnimInst->Montage_JumpToSection(TEXT("Push_Exit"), PullPushMontage);
}


void AItTakesTwoCharacter::SetBaseMovementMappingContext()
{
	//EnhancedInputSubsystem->ClearAllMappings();

	if (EnumHasAnyFlags(CurrentMovementModeState, EMovementState::Climbing))
	{
		//아쉽지만 일단 두개밖에 없으니까 이렇게 지우기로
		EnhancedInputSubsystem->RemoveMappingContext(DefaultMappingContext);

		//벽타기 매핑 컨텍스트 추가 
		EnhancedInputSubsystem->AddMappingContext(ClimbingMappingContext, MoveMappingPriority);
		GetCharacterMovement()->SetMovementMode(MOVE_Custom);
		GetCharacterMovement()->GravityScale = 0.0f;
	}
	else if (EnumHasAnyFlags(CurrentMovementModeState, EMovementState::Flying))
	{
		//비행 매핑 컨텍스트 추가
	}
	else if (EnumHasAnyFlags(CurrentMovementModeState, EMovementState::Swimming))
	{
		//수영 매핑 컨텍스트 추가
	}
	else
	{
		EnhancedInputSubsystem->RemoveMappingContext(ClimbingMappingContext);

		//기본 매핑 컨텍스트
		EnhancedInputSubsystem->AddMappingContext(DefaultMappingContext, MoveMappingPriority);
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		GetCharacterMovement()->GravityScale = 1.0f;
	}
}

void AItTakesTwoCharacter::SetInteractionInputLock(bool bInputLock)
{
	if (bInputLock)
	{
		bIgnoreInteractionInput = true;
	}
	else
	{
		bIgnoreInteractionInput = false;
	}
}

void AItTakesTwoCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	InputMovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);


		if (AnimInst->RootMotionMode != ERootMotionMode::Type::RootMotionFromEverything)
		{
			// add movement 
			AddMovementInput(ForwardDirection, InputMovementVector.Y);
			AddMovementInput(RightDirection, InputMovementVector.X);
		}
	}
}

void AItTakesTwoCharacter::StopMove()
{
	InputMovementVector = FVector2D::ZeroVector;
}

void AItTakesTwoCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AItTakesTwoCharacter::Dash(const FInputActionValue& Value)
{
	if (bIgnoreMoveInput)
	{
		return;
	}

	if (bCanDash && DashMontage)
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0, ControlRotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		FVector DashDirection;

		if (InputMovementVector.IsNearlyZero())
		{
			SetActorRotation(YawRotation);
		}
		else
		{
			DashDirection = ForwardDirection * InputMovementVector.Y + RightDirection * InputMovementVector.X;
			SetActorRotation(DashDirection.Rotation());
		}

		UE_LOG(LogTemp, Warning, TEXT("%f %f"), DashDirection.X, DashDirection.Y);

		PlayAnimMontage(DashMontage);
		SetIgnoreInputPlayingMontage(DashMontage);
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Dash"));
	}
}

void AItTakesTwoCharacter::CustomJump(const FInputActionValue& Value)
{
	if (bIgnoreMoveInput)
	{
		return;
	}

	if (bCanJump)
	{
		Super::Jump();
	}
}

void AItTakesTwoCharacter::CustomStopJumping()
{
	Super::StopJumping();
}

void AItTakesTwoCharacter::CustomInterAction(const FInputActionValue& Value)
{
	if (bIgnoreInteractionInput)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("CustomInteraction"));

	if (GrabInterActionComponent != nullptr)
	{
		GrabInterActionComponent->CustomInterAction(PickUpItem);
	}
}

void AItTakesTwoCharacter::CustomCrouch(const FInputActionValue& Value)
{
	if (bIgnoreMoveInput)
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("CustomCrouch"));

	//공중에 떠있는 상태라면 내려찍기 하고 return
	if (GetCharacterMovement() || GroundPoundMontage)
	{
		if (GetCharacterMovement()->IsFalling())
		{
			if (CustomCharacterMovementComp == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("내려찍기 중 커스텀 무브먼트 컴포넌트를 찾지 못함."));
			}

			CurrentGroundPoundCount = 0;
			CustomCharacterMovementComp->PrePoundGravityScale = GetCharacterMovement()->GravityScale;

			PlayAnimMontage(GroundPoundMontage);
			SetIgnoreInputPlayingMontage(GroundPoundMontage);
			UE_LOG(LogTemp, Warning, TEXT("내려찍기!"));
			return;
		}
	}

	//내려찍기를 하지 않았다면 웅크리기
	if (EnumHasAnyFlags(CurrentMovementModeState, EMovementState::Crouch))
	{
		//웅크리기 해제

		//머리 위에 공간이 있는지 확인
		float PreCapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		float RayLenght = DefaultCapsuleHalfHeight - PreCapsuleHalfHeight;

		FVector StartVec = GetCapsuleComponent()->GetComponentLocation() + (GetActorUpVector() * GetCapsuleComponent()->
			GetScaledCapsuleHalfHeight());
		FVector EndVec = StartVec + GetActorUpVector() * RayLenght;

		if (CheckLineTrace(StartVec, EndVec))
		{
			//뭔가 가로막고 있으므로 일어나지 않음.
			UE_LOG(LogTemp, Warning, TEXT("뭔가 가로 막고 있어서 일어나지 않음"));
			return;
		}

		CurrentMovementModeState &= ~EMovementState::Crouch;

		GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCapsuleHalfHeight);

		FVector NewLocation = GetMesh()->GetRelativeLocation();
		float NewMeshocation = PreCapsuleHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		NewLocation.Z += NewMeshocation;

		GetMesh()->SetRelativeLocation(NewLocation);
	}
	else
	{
		//웅크리기
		CurrentMovementModeState |= EMovementState::Crouch;

		DefaultCapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCapsuleHalfHeight * 0.75f);

		FVector NewLocation = GetMesh()->GetRelativeLocation();
		float NewMeshocation = DefaultCapsuleHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		NewLocation.Z += NewMeshocation;

		GetMesh()->SetRelativeLocation(NewLocation);
	}

	// //캐릭터 콜리전 크기를 피직스 에셋을 기준으로 변경. 애니메이션이 다 나오기 전에 계산을 해버려서 사용하지 않기로 결정.
	// FBox HeadBounds = GetMesh()->GetBodyInstance(TEXT("Head"))->GetBodyBounds();
	// float HighestPoint = HeadBounds.Max.Z; // 피직스 바디의 가장 윗면 Z값
	// 	
	// //발과 머리를 기준으로 계산. 각자 장단점이 있어서 그냥 기록용으로 남겨둠.
	// //FBox FootBounds = GetMesh()->GetBodyInstance(TEXT("RightFoot"))->GetBodyBounds();
	// //float LowestPoint = FootBounds.Min.Z;
	// 	
	// float GroundZ = GetActorLocation().Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	// 	
	// float NewHight = HighestPoint - GroundZ;
	// //float NewHight = HighestPoint - LowestPoint;
	//
	// GetCapsuleComponent()->SetCapsuleHalfHeight(NewHight * 0.5f);
}

void AItTakesTwoCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (AnimInst == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Landed의 AnimInst가 없습니다"));
		return;;
	}


	if (GroundPoundMontage && AnimInst->Montage_IsPlaying(GroundPoundMontage))
	{
		if (TryGroundPoundBreak(Hit))
		{
			CurrentGroundPoundCount++;

			GetCharacterMovement()->StopMovementImmediately();
			//GetCharacterMovement()->Velocity = GetActorUpVector() * -1 * CustomCharacterMovementComp->GroundPoundSpeed;	//즉시 빨라지는 느낌이 아니라 끊기는 느낌이 심하다.

			FVector LaunchVel = GetActorUpVector() * -1.f * CustomCharacterMovementComp->GroundPoundSpeed;

			// XY, Z 속도 무시하고 발사
			LaunchCharacter(LaunchVel, true, true);

			UE_LOG(LogTemp, Warning, TEXT("%s"), *GetCharacterMovement()->Velocity.ToString());
		}
		else
		{
			AnimInst->Montage_JumpToSection(TEXT("GroundPound_Land"), GroundPoundMontage);
			return;
		}
	}
}

void AItTakesTwoCharacter::Climb(const FInputActionValue& Value)
{
	if (bIgnoreMoveInput)
	{
		return;
	}

	// input is a Vector2D
	InputMovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		FVector UpVector = GetActorUpVector();

		FVector WallRight = FVector::CrossProduct(WallNormal, UpVector).GetSafeNormal();
		FVector WallUp = FVector::CrossProduct(WallRight, WallNormal).GetSafeNormal();

		// add movement 
		AddMovementInput(WallUp, InputMovementVector.Y);
		AddMovementInput(WallRight, InputMovementVector.X);

		if (InputMovementVector.Y > 0)
		{
			TryClimbUp();
		}
	}
}
