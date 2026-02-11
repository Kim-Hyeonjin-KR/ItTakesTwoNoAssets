// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItTakesTwo/ItTakesTwoCharacter.h"
#include "CodyCharacter.generated.h"

class UNailWeaponComponent;

/**
 * 
 */
UCLASS()
class ITTAKESTWO_API ACodyCharacter : public AItTakesTwoCharacter
{
	GENERATED_BODY()
	
public:
	ACodyCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CustomComponent)
	UNailWeaponComponent* NailWeaponComponent;
};
