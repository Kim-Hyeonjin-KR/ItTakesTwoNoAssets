// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ElectronicInterface.h"
#include "ElectronicActor.generated.h"

UCLASS()
class ITTAKESTWO_API AElectronicActor : public AActor, public IElectronicInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AElectronicActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnSignal_Implementation(AActor* Interactor, bool bSignal) override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnActivate(AActor* SelfActor);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnDeActivate(AActor* SelfActor);
	
private:
	
	//전기 오브젝트에 연결된 전원부들과 활성화 여부 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	TMap<AActor*, bool> LinkedSwitchs;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bAllSwitchsActive;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	FRotator InitialRotation;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bAnySwitchsCanActive;
};
