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
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ItemType))
	EPickUpItemType PickUpItemType;	
	
public:
	virtual EPickUpItemType PickUpItemInteract_Implementation(AActor* Interactor) override;
	virtual void PutDownItemInteract_Implementation(AActor* Interactor) override;
	
	
	void AttachItem(AActor* Target);
	void DetachItem(AActor* Target);
	
private:
	USkeletalMeshComponent* GetTargetMesh(AActor* Target);
};
