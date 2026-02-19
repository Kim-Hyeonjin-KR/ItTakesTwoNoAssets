// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/HammerWeaponComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Component/NailWeaponComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ItTakesTwo/ItTakesTwoCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon/Hammer.h"
#include "Weapon/Nail.h"

// Sets default values for this component's properties
UHammerWeaponComponent::UHammerWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	
	PrimaryComponentTick.bCanEverTick = true;     // 틱을 가질 수 있는 컴포넌트임을 선언
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// ...
}


// Called when the game starts
void UHammerWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	
	AddHammer();
	
	OwnerCharacter = Cast<AItTakesTwoCharacter>(GetOwner());
	
	ensure(OwnerCharacter.Get());
	ensure(HammerAttackMontage);
	ensure(HammerSwingMontage);
}


// Called every frame
void UHammerWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	/*
	if (OwnerCharacter.IsValid() == false) { return;}
	
	// 1. 현재 캐릭터의 월드 회전을 가져옴
	FRotator CurrentRot = OwnerCharacter->GetActorRotation();

	// 2. Pitch와 Roll은 0으로, Yaw만 유지한 새로운 회전값 생성
	FRotator TargetRot = FRotator(0.f, CurrentRot.Yaw, 0.f);

	// 3. 캐릭터에게 적용
	OwnerCharacter->SetActorRotation(TargetRot);
	*/
	//OwnerCharacter->SetActorRotation()
	
	/*
	if (SwingTargetNail.IsValid() == false) { return; }

	//Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//스윙 로직
	//타겟 네일 회전 값 가져오기
	CurrentRoll = SwingTargetNail->GetActorRotation().Roll;

	//전진
	if (SwingVector.Y > 0)
	{
		SwingDirection = 1;
		if (CurrentRoll > MaxAngle)
		{
			SwingDirection = -1;
			//반대로 후진
			SwingTargetNail->AddActorLocalRotation(FRotator(0, 0, SwingVector.Y * SwingSpeed));
		}
		//최대 값 보다 크지 않다면 그대로 전진
	}
	//후진
	else if (SwingVector.Y < 0)
	{
		SwingDirection = -1;
		if (CurrentRoll < MinAngle)
		{
			//반대로 전진
			SwingDirection = 1;
		}
		//최소 값 보다 작지 않다면 그대로 후진
	}
	
	//입력 값이 없다면 내려가기
	if (SwingVector.Y == 0)
	{
		UE_LOG(LogTemp,Warning,TEXT("입력 값 없음"));
		InitRotation = FMath::RInterpTo(SwingTargetNail->GetActorRotation(), SwingTargetNail->PinnedInitRotation, DeltaTime, 5.0f);
		SwingTargetNail->SetActorRelativeRotation(InitRotation);
		SwingVector = FVector2D::ZeroVector;
		return;
	}
	
	//역방향이면 반대로
	if (bRightSideStart == false)
	{
		UE_LOG(LogTemp,Warning,TEXT("역방향"));
		SwingDirection = SwingDirection * -1;
	}
	UE_LOG(LogTemp,Warning,TEXT("회전"));
	SwingTargetNail->AddActorLocalRotation(FRotator(0,0,SwingDirection * SwingVector.Y * SwingSpeed));
	
	SwingVector = FVector2D::ZeroVector;
	*/
}

void UHammerWeaponComponent::SetupHammerActionInput(class UInputComponent* PlayerInputComponent)
{
	if (OwnerCharacter.Get() == nullptr)
	{
		OwnerCharacter = Cast<AItTakesTwoCharacter>(GetOwner());
		ensure(OwnerCharacter.Get());
	};
	ensureMsgf(OwnerCharacter.IsValid(), TEXT("UHammerWeaponComponent %s가 null 입니다."), *GetName());
	
	if (APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController()))
	{
		EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (EnhancedInputSubsystem != nullptr)
		{
			EnhancedInputSubsystem->AddMappingContext(HammerMappingContext, WeaponPriority);
			
			//EnhancedInputSubsystem->AddMappingContext(HammerSwingMappingContext, SwingPriority); //필요할 때 Set방식으로 변경
			//EnhancedInputSubsystem->RemoveMappingContext(HammerMappingContext);
		}
	}
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &UHammerWeaponComponent::Attack);
		
		EnhancedInputComponent->BindAction(SwingMoveAction, ETriggerEvent::Triggered, this, &UHammerWeaponComponent::SwingMove);
		EnhancedInputComponent->BindAction(SwingJumpAction, ETriggerEvent::Started, this, &UHammerWeaponComponent::SwingJump);
	}
}

void UHammerWeaponComponent::SetHammerCollision(bool bCollisionOn)
{
	if (Hammer == nullptr) { UE_LOG(LogTemp, Warning, TEXT("UHammerWeaponComponent 의 SetHammerCollision 함수. Hammer 이 없습니다")); return; }
	if (Hammer->HeadCollision == nullptr) { UE_LOG(LogTemp, Warning, TEXT("UHammerWeaponComponent 의 SetHammerCollision 함수. HeadCollision 이 없습니다")); return; }
	
	if (bCollisionOn)
	{
		Hammer->HeadCollision->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	}
	else
	{
		Hammer->HeadCollision->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
}

void UHammerWeaponComponent::Attack()
{
	UE_LOG(LogTemp,Warning,TEXT("헤머 어택"));
	
	if (OwnerCharacter.IsValid() == false) { UE_LOG(LogTemp, Warning, TEXT("HammerWeaponComponent의 OwnerCharacter이 없습니다.")) return;}
	if (OwnerCharacter->GetIgnoreMoveInput() || OwnerCharacter->GetIgnoreInteractionInput()) { return; }
	
	UCharacterMovementComponent* CharacterMovementComponent = Cast<UCharacterMovementComponent>(OwnerCharacter->GetCharacterMovement());
	if (CharacterMovementComponent == nullptr) { return; }
	if (OwnerCharacter->GetCharacterMovement()->MovementMode != MOVE_Walking) { return; }
	if (HammerAttackMontage == nullptr) { UE_LOG(LogTemp,Warning,TEXT("HammerAttackMontage가 없습니다.")) return;}
	
	OwnerCharacter->PlayAnimMontage(HammerAttackMontage);
}

void UHammerWeaponComponent::SwingMove(const FInputActionValue& Value)
{
	UE_LOG(LogTemp,Warning,TEXT("스윙"));
	
	//if (OwnerCharacter.IsValid() == false) { return; }
	//SwingVector = Value.Get<FVector2D>();
}

void UHammerWeaponComponent::SwingJump(const FInputActionValue& Value)
{
	UE_LOG(LogTemp,Warning,TEXT("스윙 점프"));
	EndGrabNail();
	if (OwnerCharacter.IsValid())
	{
		OwnerCharacter->PlayAnimMontage(HammerSwingMontage,1,"Exit");
	}
}

void UHammerWeaponComponent::AddHammer()
{
	Hammer = GetWorld()->SpawnActor<AHammer>(HammerClass, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
	Hammer->SetOwner(GetOwner());
	Hammer->SetHammerOwnerCharacter(GetOwner());
	Hammer->AttachHammer("NailSocket");
}

void UHammerWeaponComponent::ChangeSocket(FName TargetSocketName)
{
	Hammer->AttachHammer(TargetSocketName);
}

void UHammerWeaponComponent::SetSwingMappingContext(bool bActive)
{
	if (bActive)
	{
		EnhancedInputSubsystem->AddMappingContext(SwingMappingContext, SwingPriority);
	}
	else
	{
		EnhancedInputSubsystem->RemoveMappingContext(SwingMappingContext);
	}
}

void UHammerWeaponComponent::GrabNail(ANail* TargetNail, bool bRightSide)
{
	if (TargetNail == nullptr) { return; }
	
	//못이 리콜 될 때 부착 해제 이벤트 바인딩
	TargetNail->OnRecall.BindUObject(this, &UHammerWeaponComponent::EndGrabNail);
	
	//스윙 계산을 위한 설정
	bSwing = true;
	bRightSideStart = bRightSide;
	SwingTargetNail = TargetNail;
	MaxAngle = TargetNail->PinnedInitRotation.Roll + AngleLimit;
	MinAngle = TargetNail->PinnedInitRotation.Roll - AngleLimit;
	
	//캐릭터 설정
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_None);
	OwnerCharacter->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	
	/*
	FAttachmentTransformRules CustomRules(
	EAttachmentRule::SnapToTarget,
	EAttachmentRule::KeepWorld,
	EAttachmentRule::KeepWorld,
	false 
	); 
	OwnerCharacter->AttachToComponent(TargetNail->NailSkeletalMesh, CustomRules, FName("Head"));
	*/
	
	OwnerCharacter->AttachToComponent(TargetNail->NailSkeletalMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Head"));
	SetSwingMappingContext(true);
	
	//블루 프린트에서 캐릭터를 방향에 맞게 회전
	OnGrabNailDelegate.Broadcast(OwnerCharacter.Get());

	//OwnerCharacter->GetRootComponent()->SetAbsolute(false, true, false);
	
	//잡기 애니메이션 출력
	if (OwnerCharacter.IsValid())
	{
		OwnerCharacter->PlayAnimMontage(HammerSwingMontage,1,"Enter");
	}
	
	//스윙 로직을 활성화 (구현 포기)
	PrimaryComponentTick.SetTickFunctionEnable(true);
}

void UHammerWeaponComponent::EndGrabNail()
{
	PrimaryComponentTick.SetTickFunctionEnable(false);
	
	bSwing = false;
	SwingTargetNail = nullptr;
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	OwnerCharacter->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	OwnerCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetSwingMappingContext(false);
	OnEndGrabNailDelegate.Broadcast();
}

void UHammerWeaponComponent::SetCollisionActive(bool bCollisionSet)
{
	if (bCollisionSet)
	{
		Hammer->HeadCollision->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	}
	else
	{
		Hammer->HeadCollision->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
}
