// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CodyCharacter.h"

#include "EnhancedInputComponent.h"
#include "ItTakesTwo/Public/Component/NailWeaponComponent.h"


ACodyCharacter::ACodyCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NailWeaponComponent = CreateDefaultSubobject<UNailWeaponComponent>(TEXT("NailWeapon"));
	
}

void ACodyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (NailWeaponComponent != nullptr)
	{
		//몽타주 재생 등 캐릭터가 해야 하는 반응 추가
		//NailWeaponComponent->OnItemPickUp.BindUObject(this, &AItTakesTwoCharacter::SetPickUpItemType);
		
		//NailWeaponComponent
	}
}

void ACodyCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
		{
			// SpecialAbility 못 공격, 조준
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ACodyCharacter::ShotingNail);
			//EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ACodyCharacter::AimmingNail);
		}
	//스폰한 못을 관리
	
	
	
	
}

void ACodyCharacter::ShotingNail(const FInputActionValue& Value)
{
	if (bIgnoreInteractionInput || bIgnoreMoveInput)
	{
		return;
	}
	
	if (NailWeaponComponent != nullptr)
	{
		NailWeaponComponent->ShotingNail();
	}
	
	//if (NailShotMontage == nullptr) { UE_LOG(LogTemp, Warning, TEXT("NailShotMontage == nullptr")); return; }
	
	UE_LOG(LogTemp, Warning, TEXT("NailShot!!"));
}
