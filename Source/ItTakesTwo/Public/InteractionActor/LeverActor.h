// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/GrabInterableInterface.h"
#include "LeverActor.generated.h"

UCLASS()
class ITTAKESTWO_API ALeverActor : public AActor, public IGrabInterableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALeverActor();
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ItemType))
	EHandItemType ItemType = EHandItemType::ToggleLever;	
	
	virtual EHandItemType ActiveItemInteract_Implementation(AActor* Interactor) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bActive;
	
	bool IsHandleTilteLeft();
	
private:
	void ActiveLever();
	void SendSignal();
	
	bool bHandleTiltLeft;
	
	//레버 엑터에 연결된 전기 오브젝트들 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	TArray<AActor*> LinkedElectronics;
};
