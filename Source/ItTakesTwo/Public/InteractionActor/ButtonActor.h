// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/GrabInterableInterface.h"
#include "ButtonActor.generated.h"

class AElectronicActor;

UCLASS()
class ITTAKESTWO_API AButtonActor : public AActor, public IGrabInterableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AButtonActor();
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EHandItemType ItemType = 	EHandItemType::HoldButton;
	
	virtual EHandItemType ActiveItemInteract_Implementation(AActor* Interactor) override;
	virtual void DeactiveItemInteract_Implementation(AActor* Interactor) override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bActive;
	
private:
	void ToggleButtonActive();
	void HoldButtonActive();
	void ReleaseHoldButton();
	
	void SendSignal();
	
	//버튼 엑터에 연결된 전기 오브젝트들 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	TArray<AActor*> LinkedElectronics;
};
