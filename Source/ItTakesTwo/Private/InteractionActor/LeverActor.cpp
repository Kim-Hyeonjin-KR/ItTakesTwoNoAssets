// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionActor/LeverActor.h"
#include "Interface/ElectronicInterface.h"

// Sets default values
ALeverActor::ALeverActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ALeverActor::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (ItemType == EHandItemType::ToggleLever)
	{
		return;
	}
	else
	{
		FMessageLog("EditorErrors").Warning(FText::FromString("레버는 ToggleLever만 선택해주세요"));
		ItemType = EHandItemType::ToggleLever;
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

EHandItemType ALeverActor::ActiveItemInteract_Implementation(AActor* Interactor)
{
	ActiveLever();
	
	return ItemType;
}

bool ALeverActor::IsHandleTilteLeft()
{
	return bHandleTiltLeft;
}

void ALeverActor::ActiveLever()
{
	UE_LOG(LogTemp, Log, TEXT("GrabLever"));
	
	if (bActive)
	{
		bActive = false;
		UE_LOG(LogTemp, Log, TEXT("레버 비활성화"));
	}
	else
	{
		bActive = true;
		UE_LOG(LogTemp, Log, TEXT("레버 활성화"));
	}
	
	SendSignal();
	
}

void ALeverActor::SendSignal()
{
	//연결된 전자기기에 신호 전달
	for (const auto& ElectronicActor : LinkedElectronics)
	{
		if (ElectronicActor->Implements<UElectronicInterface>())
		{
			IElectronicInterface::Execute_OnSignal(ElectronicActor, this, bActive);
		}
	}
}

