// Fill out your copyright notice in the Description page of Project Settings.

#include "ItTakesTwo/Public/Anim/ANSInvincibility.h"
#include "ItTakesTwo/ItTakesTwoCharacter.h"

void UANSInvincibility::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	AItTakesTwoCharacter* Player = Cast<AItTakesTwoCharacter>(MeshComp->GetOwner());
	Player->ECharacterAction = ECharacterActionType::Dash;
}

void UANSInvincibility::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	AItTakesTwoCharacter* Player = Cast<AItTakesTwoCharacter>(MeshComp->GetOwner());
	Player->ECharacterAction = ECharacterActionType::Idle;
}
