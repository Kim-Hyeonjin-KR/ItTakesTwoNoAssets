// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NailWeaponComponent.generated.h"

class ANail;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ITTAKESTWO_API UNailWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNailWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category= "Nail")
	void AddNail();
	
	UFUNCTION(BlueprintCallable, Category= "Nail")
	bool ShotingNail();
	
	UFUNCTION(BlueprintCallable, Category = "Nail")
	void RecallNail(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Nail")
	void TryRecallAllNail();
	
	UPROPERTY(BlueprintReadOnly, Category= "Nail")
	int MaxNailCount = 3;
	
	UPROPERTY(EditAnywhere, Category= "Nail")
	TSubclassOf<ANail> NailClass;
	
	UPROPERTY(EditAnywhere, Category = "Nail")
	TArray<ANail*> Nails;
	
	
};
