// Fill out your copyright notice in the Description page of Project Settings.

#include "ItTakesTwo/Public/Anim/ANSInvincibility.h"
#include "ItTakesTwo/ItTakesTwoCharacter.h"

void UANSInvincibility::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	AItTakesTwoCharacter* Player = Cast<AItTakesTwoCharacter>(MeshComp->GetOwner());
	if (Player != nullptr)
	{
		Player->CurrentMovementModeState |= EMovementState::Dash;
		Player->bCanDash = false;
		DashLength = Player->DashLength;
		
		FVector LastInput = Player->GetLastMovementInputVector(); 
		if (LastInput.IsNearlyZero() != true)
		{
			DashDirection = LastInput;
		}
		else
		{
			DashDirection = Player->GetActorForwardVector();
		}
		DashDirection.GetSafeNormal();
		DashDirection.Z = 0.0f;
		
		
		//DashDirection = FRotationMatrix(CurrentRotation).GetUnitAxis(EAxis::X);
		
		if (TotalDuration > 0.0f)
		{
			DashSpeed = DashLength / TotalDuration;
		}
		
		Player->GetController()->SetIgnoreMoveInput(true);
		Player->bCanJump = false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ANS Invincibility Begin에서 Player 찾을 수 없음."));
	}
}

void UANSInvincibility::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	AItTakesTwoCharacter* Player = Cast<AItTakesTwoCharacter>(MeshComp->GetOwner());
	
	if (Player != nullptr)
	{
		Player->CurrentMovementModeState &= ~EMovementState::Dash;
		Player->bCanDash = true;
		
		Player->GetController()->SetIgnoreMoveInput(false);
		Player->bCanJump = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ANS Invincibility End에서 Player 찾을 수 없음."));
	}
}

void UANSInvincibility::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	
	
	if (AActor* Owner = MeshComp->GetOwner())
	{
		FVector DeltaLocation = DashDirection * DashSpeed * FrameDeltaTime;
		
		Owner->AddActorWorldOffset(DeltaLocation, true);
		
	}
	
}
