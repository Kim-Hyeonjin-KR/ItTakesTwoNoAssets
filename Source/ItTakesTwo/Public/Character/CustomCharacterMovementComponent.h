// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomCharacterMovementComponent.generated.h"

/**
 * 
 */

enum ECustomMovementMode
{
	CMOVE_Climbing
};


UCLASS()
class ITTAKESTWO_API UCustomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode);
	
	bool bHasDefaultFlySetting;
	float DefaultFlyingModeBrakingDecelFlying;
	float DefaultFlyingModeAirControl;
	float DefaultFlyingModeGroundFriction;
	float DefaultFlyingModeGravityScale;
	
public:
	UPROPERTY(BlueprintReadOnly)
	float PrePoundGravityScale = 1.0f;

	UPROPERTY(BlueprintReadOnly)
	float GroundPoundSpeed = 2000.0f;
};
