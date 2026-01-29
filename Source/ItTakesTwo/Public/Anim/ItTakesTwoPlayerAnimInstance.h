// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ItTakesTwo\ItTakesTwoCharacter.h"
#include "ItTakesTwoPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ITTAKESTWO_API UItTakesTwoPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnLinkAnimClassLayers(const EPickUpItemType& PickUpItemType);
	
};
