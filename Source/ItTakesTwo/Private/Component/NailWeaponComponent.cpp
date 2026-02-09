// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/NailWeaponComponent.h"
#include "Weapon/Nail.h"

// Sets default values for this component's properties
UNailWeaponComponent::UNailWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UNailWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UNailWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UNailWeaponComponent::AddNail(ANail* NailActor)
{
	if (IsValid(NailActor) == false) { UE_LOG(LogTemp, Error, TEXT("Nail Actor is invalid")); return; }
	
	ANail* NewNail = Cast<ANail>(NailActor);
	
	if (NewNail != nullptr)
	{
			Nails.Add(NailActor);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Can't add Nail Actor"));
	}
}


void UNailWeaponComponent::RecallNail()
{
	for (ANail* Nail : Nails)
	{
		if (IsValid(Nail))
		{
			if (Nail->GetState() == ENailState::Pinned)
			{
				Nail->Recalling();
			}
		}
	}
}

