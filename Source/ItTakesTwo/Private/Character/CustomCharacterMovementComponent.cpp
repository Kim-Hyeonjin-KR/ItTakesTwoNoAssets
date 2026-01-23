// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CustomCharacterMovementComponent.h"

void UCustomCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	if (CustomMovementMode == CMOVE_Climbing)
	{
		if (Acceleration.IsZero())
		{
			Velocity = FVector::ZeroVector;
		}
		
		PhysFlying(deltaTime, Iterations);
	}
}

void UCustomCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode,
	uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	
	if (MovementMode == MOVE_Custom && CustomMovementMode == CMOVE_Climbing)
	{
		if (bHasDefaultFlySetting == false)
		{
			DefaultBrakingDecelFlying = BrakingDecelerationFlying;
			DefaultAirControl = AirControl;
			DefaultGroundFriction = GroundFriction;
			DefaultGravityScale = GravityScale;
			bHasDefaultFlySetting = true;
		}
		
		bOrientRotationToMovement = false;
		// 아래 주석 부분 상황 봐서 지울것
		// BrakingDecelerationFlying = 100000.0f;	//급제동
		// AirControl = 1.0f;		//공중 제어력
		// //GroundFriction = 5000.0f;	//마찰력
		// GravityScale = 0.0f;	//중력. 커스텀에선 보통 영향x
		Velocity = FVector::ZeroVector;
		Acceleration = FVector::ZeroVector;
		UpdateComponentVelocity();
		StopMovementImmediately();
		
		
		
	}
	else if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == CMOVE_Climbing)
	{
		bOrientRotationToMovement = true;
		BrakingDecelerationFlying = DefaultBrakingDecelFlying;
		AirControl = DefaultAirControl;
		GroundFriction = DefaultGroundFriction;
		GravityScale = DefaultGravityScale;
		bHasDefaultFlySetting = false;
	}
}
