// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/NailWeaponComponent.h"

#include "GeometryCollection/Facades/CollectionPositionTargetFacade.h"
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
	
	AddNail();
	// ...
}


// Called every frame
void UNailWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UNailWeaponComponent::AddNail()
{
	if (NailClass == nullptr) { UE_LOG(LogTemp, Error, TEXT("Nail Actor is invalid")); return; }
	
	// 못 엑터 생성 및 연동
	for (int i = 0; i < MaxNailCount; i++)
	{
		ANail* NewNailActor = GetWorld()->SpawnActor<ANail>(NailClass, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
		NewNailActor->SetOwner(GetOwner());
		NewNailActor->SetNailOwnerCharacter(GetOwner());
		Nails.Add(NewNailActor);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Nail Actor Created Success"));
}

bool UNailWeaponComponent::ShotingNail()
{
	return true;
}

void UNailWeaponComponent::RecallNail(AActor* Target)
{
	//타겟이 네일이 아니라면 모든 네일 회수 시도
	ANail* TargetNail = Cast<ANail>(Target);
	if (TargetNail == nullptr)
	{
		TryRecallAllNail();
	}

	//타겟이 존재하고 네일이면 해당 네일만 회수
	else
	{
		if (IsValid(TargetNail) && Nails.Contains(TargetNail))
		{
			TargetNail->Recalling();
		}
	}
}

void UNailWeaponComponent::TryRecallAllNail()
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

