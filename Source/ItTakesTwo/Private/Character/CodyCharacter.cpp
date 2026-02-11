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
	
	if (NailWeaponComponent != nullptr)
	{
		//입력 바인딩 연결
		NailWeaponComponent->SetupNailActionInput(PlayerInputComponent);
	}
}
