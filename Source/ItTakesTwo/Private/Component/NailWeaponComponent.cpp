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
	FVector TargetLocation = GetAimLocation();
	
	for (ANail* Nail : Nails)
	{
		if (IsValid(Nail))
		{
			if (Nail->GetState() == ENailState::Stored)
			{
				Nail->Shoting(TargetLocation);
				return true;
			}
		}
	}
	return false;
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
			if (Nail->GetState() != ENailState::Stored)
			{
				Nail->Recalling();
			}
		}
	}
}

FVector UNailWeaponComponent::GetAimLocation()
{
	FVector CameraLocation;
	FRotator CameraRotation;
	
	//컨트롤러 연동 단계에서 로직에 맞게 수정해줄것.
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	
	FVector Direction = CameraRotation.Vector();
	FVector EndLocation = CameraLocation + (Direction * MaxAimRange);
	
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	for (auto Nail : Nails)
	{
		if (IsValid(Nail))
		{
			Params.AddIgnoredActor(Nail);
		}
	}
	
	//FVector BoxHalfSize = FVector(5.0f, CapsuleComponent->GetScaledCapsuleRadius() , CapsuleComponent->GetScaledCapsuleHalfHeight());
	//FCollisionShape MyBox = FCollisionShape::MakeBox(BoxHalfSize);
	
	GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, EndLocation, ECC_Visibility, Params);

	DrawDebugLine(
		GetWorld(),
		CameraLocation,
		EndLocation,
		FColor::Green,
		false,
		10.0f
	);
	
	if (HitResult.bBlockingHit)
	{
		DrawDebugBox(GetWorld(), HitResult.ImpactPoint, FVector(10), FColor::Red, false, 10.0f);
	}
	
	FVector AimPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : EndLocation;
	
	return AimPoint;
}

