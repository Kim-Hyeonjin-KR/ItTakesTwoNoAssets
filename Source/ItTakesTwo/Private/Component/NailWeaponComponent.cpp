// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/NailWeaponComponent.h"

#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
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
	if (OwnerCharacter.Get() && OwnerCharacter.Get()->GetCameraBoom())
	{
		OwnerSpringArm = OwnerCharacter.Get()->GetCameraBoom();
		InitTargetArmLength = OwnerSpringArm->TargetArmLength;
		InitTargetOffset = OwnerSpringArm->TargetOffset;
	}
	
	if (CrosshairWidget == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("NailWeaponComponent의 CrosshairWidget이 없습니다."));
		UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
	}
	CrosshairWidget->AddToViewport();
	// 처음엔 안 보이게 하고 싶다면
	CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
	OwnerCharacter->SetAnimLayer(EHandItemType::Nail);
	
	// ...
}

void UNailWeaponComponent::SetupNailActionInput(class UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// SpecialAbility 못 공격, 조준
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &UNailWeaponComponent::ShotingNail);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &UNailWeaponComponent::Aiming);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &UNailWeaponComponent::Aiming);
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
		
		CrosshairWidget->SetVisibility(ESlateVisibility::Visible);
		
		//발사 대기 슬롯으로 이동 명령
		for (ANail* Nail : Nails)
		{
			if (IsValid(Nail))
			{
				if (Nail->GetState() == ENailState::Stored)
				{
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
		
		CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
		
		for (ANail* Nail : Nails)
		{
			if (IsValid(Nail))
			{
				if (Nail->GetState() == ENailState::Ready)
				{
					Nail->Store();
					return;
				}
			}
		}
		//보관 몽타주 재생?
		
	}
}


// Called every frame
void UNailWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	UE_LOG(LogTemp,Warning,TEXT("네일 틱 활성화"));
	
	float FinalArmLength;
	FVector FinalTargetOffset;
	float AimBlendValue;
	
	if (bHoldingAimButton)
	{
		FinalArmLength = 200.0f;	
		FinalTargetOffset = FVector(0,150,0);
		AimBlendValue = 1.f;
	}
	else
	{
		FinalArmLength = InitTargetArmLength;
		FinalTargetOffset = InitTargetOffset;
		AimBlendValue = 0.f;
	}
	
	OwnerSpringArm->TargetArmLength = FMath::FInterpTo(OwnerSpringArm->TargetArmLength, FinalArmLength, DeltaTime, 5.f);
	OwnerSpringArm->TargetOffset = FMath::VInterpTo(OwnerSpringArm->TargetOffset, FinalTargetOffset, DeltaTime, 5.f);
	//AimBlendValue = FMath::FInterpConstantTo(, AimBlendValue , DeltaTime, 0.0f); 
	
	//조건 충족시 틱 종료
	if (FMath::IsNearlyEqual(OwnerSpringArm->TargetArmLength, FinalArmLength, 0.5f) 
		&& (OwnerSpringArm->TargetOffset.Equals(FinalTargetOffset, 0.5f)))
	{
		SetComponentTickEnabled(false);
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
		NewNailActor->Store();
		Nails.Add(NewNailActor);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Nail Actor Created Success"));
}

void UNailWeaponComponent::RecallNail(AActor* Target)
{
	//타겟이 네일이 아니라면 모든 네일 회수 시도
	ANail* TargetNail = Cast<ANail>(Target);
	if (TargetNail == nullptr)
	{
		TryRecallAllNail();
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

void UNailWeaponComponent::TryRecallAllNail()
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



