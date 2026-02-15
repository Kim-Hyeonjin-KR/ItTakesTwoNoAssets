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
}

void ACodyCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (NailWeaponComponent != nullptr)
	{
		//입력 바인딩 연결
		NailWeaponComponent->SetupNailActionInput(PlayerInputComponent);
	}
}
