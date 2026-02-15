// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HammerWeaponComponent.generated.h"

class UInputAction;
class AItTakesTwoCharacter;
class UInputMappingContext;
class UEnhancedInputLocalPlayerSubsystem;
class AHammer;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ITTAKESTWO_API UHammerWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHammerWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY()
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetupHammerActionInput(class UInputComponent* PlayerInputComponent);
	
	UFUNCTION(BlueprintCallable, Category = "Hammer")
	void SetHammerCollision(bool bCollisionOn);
	
	UPROPERTY(EditAnywhere, Category= "Hammer", meta=(AllowPrivateAccess=true))
	TSubclassOf<AHammer> HammerClass;
	
	UPROPERTY(EditAnywhere, Category= "Hammer")
	UAnimMontage* HammerAttackMontage;
	
	UFUNCTION(BlueprintCallable, Category = "Hammer")
	void Attack();
	
	UFUNCTION(BlueprintCallable, Category = "Hammer")
	void AddHammer();
	
	UFUNCTION(BlueprintCallable, Category = "Hammer")
	void ChangeSocket(FName TargetSocketName);
	
private:
	TWeakObjectPtr<AItTakesTwoCharacter> OwnerCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* HammerMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ClimbingInput, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;
	
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void SetCollisionActive(bool bCollisionSet);
	
	UPROPERTY()
	AHammer* Hammer;
};
