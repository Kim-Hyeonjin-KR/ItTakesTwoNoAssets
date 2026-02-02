// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionActor/ElectronicActor.h"

// Sets default values
AElectronicActor::AElectronicActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AElectronicActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AElectronicActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AElectronicActor::OnSignal_Implementation(AActor* Interactor, bool bSignal)
{
	IElectronicInterface::OnSignal_Implementation(Interactor, bSignal);
	
	LinkedSwitchs.Add(Interactor, bSignal);
	
	for (auto& Switchs : LinkedSwitchs)
	{
		if (Switchs.Key == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ElectronicActor : No Linked Switch Found"));
			return;
		}
		
		if (Switchs.Value == false)
		{
			bAllSwitchsActive = false;
			OnDeActivate(this);
			return;
		}
	}
	bAllSwitchsActive = true;
	OnActivate(this);
}

void AElectronicActor::OnDeActivate_Implementation(AActor* SelfActor)
{
	
}

void AElectronicActor::OnActivate_Implementation(AActor* SelfActor)
{
	
}


