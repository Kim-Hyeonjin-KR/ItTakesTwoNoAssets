// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/GrabInterableInterface.h"
#include "PullableActor.generated.h"

UCLASS()
class ITTAKESTWO_API APullableActor : public AActor, public IGrabInterableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APullableActor();
	//virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ValidEnumValues = "PullableObject"))
	EHandItemType ItemType = EHandItemType::PullableObject;	
	
	virtual EHandItemType ActiveItemInteract_Implementation(AActor* Interactor) override;
	virtual void DeactiveItemInteract_Implementation(AActor* Interactor) override;
	
private:
	void GrabPullableActor();
	void ReleasePullableActor();
};
