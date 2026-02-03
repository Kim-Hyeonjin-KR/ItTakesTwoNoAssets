// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/GrabInterableInterface.h"
#include "ItTakesTwo/ItTakesTwoTypes.h"
#include "GrabableActor.generated.h"

class USkeletalMeshComponent;

UCLASS()
class ITTAKESTWO_API AGrabableActor : public AActor, public IGrabInterableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrabableActor();
	virtual void OnConstruction(const FTransform& Transform) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ItemType))
	EHandItemType ItemType;	
	
	virtual EHandItemType ActiveItemInteract_Implementation(AActor* Interactor) override;
	virtual void DeactiveItemInteract_Implementation(AActor* Interactor) override;
	
private:
	void AttachItem(AActor* Target);
	void DetachItem(AActor* Target);
	
	USkeletalMeshComponent* GetTargetMesh(AActor* Target);
};
