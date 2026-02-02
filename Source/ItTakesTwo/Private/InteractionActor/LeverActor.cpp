// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionActor/LeverActor.h"

// Sets default values
ALeverActor::ALeverActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ALeverActor::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (ItemType == EPickUpItemType::HoldLever)
	{
		return;
	}
	else
	{
		FMessageLog("EditorErrors").Warning(FText::FromString("레버는 HoldLever만 선택해주세요"));
		ItemType = EPickUpItemType::HoldLever;
	}
}


// Called when the game starts or when spawned
void ALeverActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALeverActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

EPickUpItemType ALeverActor::ActiveItemInteract_Implementation(AActor* Interactor)
{
	GrabLever();
	
	return ItemType;
}

void ALeverActor::DeactiveItemInteract_Implementation(AActor* Interactor)
{
	ReleaseLever();
}

void ALeverActor::GrabLever()
{
	UE_LOG(LogTemp, Log, TEXT("GrabLever"));
}

void ALeverActor::ReleaseLever()
{
	UE_LOG(LogTemp, Log, TEXT("ReleaseLever"));
}


