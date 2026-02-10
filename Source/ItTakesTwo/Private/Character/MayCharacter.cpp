// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MayCharacter.h"
#include "ItTakesTwo/Public/Component/HammerWeaponComponent.h"

AMayCharacter::AMayCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	HammerWeaponComponent = CreateDefaultSubobject<UHammerWeaponComponent>(TEXT("HammerWeapon"));
	
}

void AMayCharacter::BeginPlay()
{
	Super::BeginPlay();
}
