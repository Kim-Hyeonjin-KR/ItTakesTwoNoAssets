// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionActor/PullableActor.h"

// Sets default values
APullableActor::APullableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void APullableActor::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (ItemType == EPickUpItemType::PullableObject)
	{
		return;
	}
	else
	{
		FMessageLog("EditorErrors").Warning(FText::FromString("끌 수 있는 물체는 PullableObject만 선택해주세요"));
		ItemType = EPickUpItemType::PullableObject;
	}
}

// Called when the game starts or when spawned
void APullableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APullableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

EPickUpItemType APullableActor::ActiveItemInteract_Implementation(AActor* Interactor)
{
	GrabPullableActor();
	
	return ItemType;
}

void APullableActor::DeactiveItemInteract_Implementation(AActor* Interactor)
{
	//인터페이스 원본은 구현한게 없지만, 혹시 모르니 하나만 남겨둠.
	//IGrabInterableInterface::DeactiveItemInteract_Implementation(Interactor);
	
	ReleasePullableActor();
}

void APullableActor::GrabPullableActor()
{
	UE_LOG(LogTemp, Log, TEXT("GrabPullableActor"));
}

void APullableActor::ReleasePullableActor()
{
	UE_LOG(LogTemp, Log, TEXT("ReleasePullableActor"));
}

