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
		Player->CharacterState |= ECharacterState::Dash;
		Player->bCanDash = false;
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
		Player->CharacterState &= ~ECharacterState::Dash;
		Player->bCanDash = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ANS Invincibility End에서 Player 찾을 수 없음."));
	}
}
