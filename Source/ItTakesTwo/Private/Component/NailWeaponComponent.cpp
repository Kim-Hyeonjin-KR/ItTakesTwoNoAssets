// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/NailWeaponComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GeometryCollection/Facades/CollectionPositionTargetFacade.h"
#include "Weapon/Nail.h"
#include "ItTakesTwo/ItTakesTwoCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UNailWeaponComponent::UNailWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	
	PrimaryComponentTick.bCanEverTick = true; 
	PrimaryComponentTick.bStartWithTickEnabled = false;;
	
	// ...
}

// Called when the game starts
void UNailWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	
	AddNail();
	
	OwnerCharacter = Cast<AItTakesTwoCharacter>(GetOwner());
	
	ensure(OwnerCharacter.Get());
	
	if (OwnerCharacter.Get()->GetCameraBoom())
	{
		OwnerSpringArm = OwnerCharacter.Get()->GetCameraBoom();
		InitTargetArmLength = OwnerSpringArm->TargetArmLength;
		InitSocketOffset = OwnerSpringArm->SocketOffset;
	}
	
	if (CrosshairWidget == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("NailWeaponComponent의 CrosshairWidget이 없습니다."));
		UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
	}
	
	// 조준 UI
	CrosshairWidget->AddToViewport();
	CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
	OwnerCharacter->SetAnimLayer(EHandItemType::Nail);
	
	// ...
}

void UNailWeaponComponent::SetupNailActionInput(class UInputComponent* PlayerInputComponent)
{
	if (OwnerCharacter.Get() == nullptr)
	{
		OwnerCharacter = Cast<AItTakesTwoCharacter>(GetOwner());
		ensure(OwnerCharacter.Get());
	};
	ensureMsgf(OwnerCharacter.IsValid(), TEXT("UNailWeaponComponent의 %s가 null 입니다."), *GetName());
	
	if (APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController()))
	{
		EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (EnhancedInputSubsystem != nullptr)
		{
			EnhancedInputSubsystem->AddMappingContext(NailMappingContext, 8);
			//EnhancedInputSubsystem->AddMappingContext(NailMappingContext, WeaponPriority);
		}
	}
		
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// SpecialAbility 못 공격, 조준
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &UNailWeaponComponent::ShotingNail);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &UNailWeaponComponent::Aiming);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &UNailWeaponComponent::Aiming);
		EnhancedInputComponent->BindAction(RecallAction, ETriggerEvent::Triggered, this, &UNailWeaponComponent::Recall);
		
	}
	//스폰한 못을 관리
}

void UNailWeaponComponent::ShotingNail(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("ShotingNail!!"));
	
	if (bHoldingAimButton == false) { return; }
	
	if (IsValid(OwnerCharacter.Get()) == false) { UE_LOG(LogTemp, Error, TEXT("NailWeaponComponent의 ShotingNail함수. OwnerCharacter 없음.")); return; }
	
	if (OwnerCharacter.Get()->GetIgnoreInteractionInput() || OwnerCharacter.Get()->GetIgnoreInteractionInput())
	{
		UE_LOG(LogTemp, Warning, TEXT("입력 무시 상태"));
		return;
	}
	
	FVector TargetLocation = GetAimLocation();
	
	for (ANail* Nail : Nails)
	{
		if (IsValid(Nail))
		{
			if (Nail->GetState() == ENailState::Ready)
			{
				Nail->Shoting(TargetLocation);
				
				//발사 몽타주 재생 요청
				OwnerCharacter->PlayAnimMontage(NailMontage,1,TEXT("Throw"));
				return;
			}
		}
	}
}

void UNailWeaponComponent::Aiming(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Aiming!!"));
	
	if (IsValid(OwnerCharacter.Get()) == false){ UE_LOG(LogTemp,Warning,TEXT("NailWeaponComponent의 Aiming 함수 OwnerCharacter이 없습니다.")); return;}

	bHoldingAimButton = Value.Get<bool>();
	
	if (bHoldingAimButton)
	{
		//OwnerCharacter.Get()->GetFollowCamera();
		UE_LOG(LogTemp, Warning, TEXT("에임 홀드중"));
		SetComponentTickEnabled(true);
		
		//마우스 회전따라 회전
		OwnerCharacter->bUseControllerRotationYaw = true;
		OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		
		OwnerCharacter->SetAnimLayer(EHandItemType::Nail);
		
		CrosshairWidget->SetVisibility(ESlateVisibility::Visible);
		
		//발사 대기 슬롯으로 이동 명령
		for (ANail* Nail : Nails)
		{
			if (IsValid(Nail))
			{
				if (Nail->GetState() == ENailState::Stored)
				{
					//장착 몽타주
					OwnerCharacter->PlayAnimMontage(NailMontage,1,TEXT("Equip"));
					Nail->Ready();
					return;
				}
			}
		}
		
		//조준 몽타주 재생?
	
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("에임 홀드 해제"));
		SetComponentTickEnabled(true);
		
		//마우스 회전따라 회전 종료
		OwnerCharacter->bUseControllerRotationYaw = false;
		OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		
		OwnerCharacter->SetAnimLayer(EHandItemType::None);
		
		CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
		
		for (ANail* Nail : Nails)
		{
			if (IsValid(Nail))
			{
				if (Nail->GetState() == ENailState::Ready)
				{
					//회수 몽타주
					OwnerCharacter->PlayAnimMontage(NailMontage,1,TEXT("UnEquip"));
					
					Nail->Store();
					return;
				}
			}
		}
		//보관 몽타주 재생?
		
	}
}

void UNailWeaponComponent::Recall(const FInputActionValue& Value)
{
	if (bHoldingAimButton == false)
	{
		RecallAllNail();
		return;
	}
}

void UNailWeaponComponent::CatchNail(ANail* RecalledNail)
{
	if (NailMontage == nullptr){UE_LOG(LogTemp,Warning,TEXT("UNailWeaponComponent 클래스 CatchNail 함수 NailMontage nullptr")); return;}
	
	OwnerCharacter->PlayAnimMontage(NailMontage,1,TEXT("Recall"));
	RecalledNail->Store();
	//NailSocket
}


// Called every frame
void UNailWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	UE_LOG(LogTemp,Warning,TEXT("네일 틱 활성화"));
	
	float FinalArmLength;
	FVector FinalSocketOffset;
	float AimBlendValue;
	
	if (bHoldingAimButton)
	{
		FinalArmLength = AimingArmLength;
		FinalSocketOffset = AimingSocketOffset;
		AimBlendValue = 1.f;
	}
	else
	{
		FinalArmLength = InitTargetArmLength;
		FinalSocketOffset = InitSocketOffset;
		AimBlendValue = 0.f;
	}
	
	OwnerSpringArm->TargetArmLength = FMath::FInterpTo(OwnerSpringArm->TargetArmLength, FinalArmLength, DeltaTime, 5.f);
	OwnerSpringArm->SocketOffset = FMath::VInterpTo(OwnerSpringArm->SocketOffset, FinalSocketOffset, DeltaTime, 5.f);
	CurrentAimBlendValue = FMath::FInterpTo(CurrentAimBlendValue , AimBlendValue , DeltaTime, 5.0f); 
	
	//조건 충족시 틱 종료
	if (FMath::IsNearlyEqual(OwnerSpringArm->TargetArmLength, FinalArmLength, 0.5f) 
		&& (OwnerSpringArm->TargetOffset.Equals(FinalSocketOffset, 0.5f)))
	{
		CurrentAimBlendValue = AimBlendValue;
		SetComponentTickEnabled(false);
		UE_LOG(LogTemp,Warning,TEXT("네일 틱 종료"));
		
	}
}

void UNailWeaponComponent::AddNail()
{
	if (NailClass == nullptr) { UE_LOG(LogTemp, Error, TEXT("Nail Actor is invalid")); return; }
	
	// 못 엑터 생성 및 연동
	for (int i = 0; i < MaxNailCount; i++)
	{
		ANail* NewNailActor = GetWorld()->SpawnActor<ANail>(NailClass, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
		NewNailActor->SetOwner(GetOwner());
		NewNailActor->SetNailOwnerCharacter(GetOwner());
		NewNailActor->StoreSocketOffset = FVector(0, i* -19,0);
		NewNailActor->Store();
		NewNailActor->OnRecallEnd.BindUObject(this, &UNailWeaponComponent::CatchNail);
		Nails.Add(NewNailActor);
	}
	UE_LOG(LogTemp, Warning, TEXT("Nail Actor Created Success"));
}

void UNailWeaponComponent::RecallTargetNail(AActor* Target)
{
	//타겟이 네일이 아니라면 모든 네일 회수 시도
	ANail* TargetNail = Cast<ANail>(Target);
	if (TargetNail == nullptr)
	{
		RecallAllNail();
	}

	//타겟이 존재하고 네일이면 해당 네일만 회수
	else
	{
		if (IsValid(TargetNail) && Nails.Contains(TargetNail))
		{
			TargetNail->Recalling();
		}
	}
}

void UNailWeaponComponent::RecallAllNail()
{
	for (ANail* Nail : Nails)
	{
		if (IsValid(Nail))
		{
			if (Nail->GetState() != ENailState::Stored)
			{
				Nail->Recalling();
			}
		}
	}
}

FVector UNailWeaponComponent::GetAimLocation()
{
	FVector CameraLocation;
	FRotator CameraRotation;
	
	//컨트롤러 연동 단계에서 로직에 맞게 수정해줄것.
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	
	FVector Direction = CameraRotation.Vector();
	FVector EndLocation = CameraLocation + (Direction * MaxAimRange);
	
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	for (auto Nail : Nails)
	{
		if (IsValid(Nail))
		{
			Params.AddIgnoredActor(Nail);
		}
	}
	
	GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, EndLocation, ECC_Visibility, Params);

	DrawDebugLine(
		GetWorld(),
		CameraLocation,
		EndLocation,
		FColor::Green,
		false,
		10.0f
	);
	
	if (HitResult.bBlockingHit)
	{
		DrawDebugBox(GetWorld(), HitResult.ImpactPoint, FVector(10), FColor::Red, false, 10.0f);
	}
	FVector AimPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : EndLocation;
	return AimPoint;
}

ANail* UNailWeaponComponent::FindPinedNail()
{
	FVector CameraLocation;
	FRotator CameraRotation;
	
	//컨트롤러 연동 단계에서 로직에 맞게 수정해줄것.
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	
	FVector Direction = CameraRotation.Vector();
	FVector EndLocation = CameraLocation + (Direction * MaxAimRange);
	
	FHitResult HitResult;
	float SphereRadius = 10.f;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(SphereRadius);
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	Params.bTraceComplex = true;
	
	//채널 추가해줄것
	GetWorld()->SweepSingleByChannel(HitResult, CameraLocation, EndLocation, FQuat::Identity, ECC_Visibility, SphereShape, Params);

	
	return nullptr;
}



