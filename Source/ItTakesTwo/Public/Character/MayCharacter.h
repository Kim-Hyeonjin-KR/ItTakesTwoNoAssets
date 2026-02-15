// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItTakesTwo/ItTakesTwoCharacter.h"
#include "MayCharacter.generated.h"

class UHammerWeaponComponent; 

/**
 * 
 */
UCLASS()
class ITTAKESTWO_API AMayCharacter : public AItTakesTwoCharacter
{
	GENERATED_BODY()
	
public:
	AMayCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CustomComponent)
	UHammerWeaponComponent* HammerWeaponComponent;
	
	
};
