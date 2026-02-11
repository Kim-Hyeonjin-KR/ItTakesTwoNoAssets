// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NailWeaponComponent.generated.h"

class USpringArmComponent;
class UInputAction;
class AItTakesTwoCharacter;
struct FInputActionValue;
class ANail;
class UUserWidget;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ITTAKESTWO_API UNailWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNailWeaponComponent();
	
	// Called when the game starts
	virtual void BeginPlay() override;
	
	void SetupNailActionInput(class UInputComponent* PlayerInputComponent);
	
	void ShotingNail(const FInputActionValue& Value);
	void Aiming(const FInputActionValue& Value);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category= "Nail")
	void AddNail();
	
	UFUNCTION(BlueprintCallable, Category = "Nail")
	void RecallNail(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Nail")
	void TryRecallAllNail();
	
	UFUNCTION(BlueprintCallable, Category= "Nail")
	FVector GetAimLocation();
	
	UPROPERTY(EditAnywhere, Category= "Nail")
	UUserWidget* CrosshairWidget;
	
	void SetInitTargetArmLength(const float _InitTargetArmLength) { InitTargetArmLength = _InitTargetArmLength; }
	void SetInitTargetOffset(const FVector& _InitTargetOffset) { InitTargetOffset = _InitTargetOffset; }
	
private:
	TWeakObjectPtr<AItTakesTwoCharacter> OwnerCharacter;
	TWeakObjectPtr<USpringArmComponent> OwnerSpringArm;
	
	float InitTargetArmLength;
	FVector InitTargetOffset;
	
	
	UPROPERTY(BlueprintReadOnly, Category= "Nail", meta=(AllowPrivateAccess=true))
	int MaxNailCount = 3;
	
	UPROPERTY(EditAnywhere, Category= "Nail", meta=(AllowPrivateAccess=true))
	float MaxAimRange = 10000.0f;
	
	UPROPERTY(EditAnywhere, Category= "Nail", meta=(AllowPrivateAccess=true))
	TSubclassOf<ANail> NailClass;
	
	UPROPERTY()
	TArray<ANail*> Nails;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ClimbingInput, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ClimbingInput, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;
	
	bool bHoldingAimButton;
};
