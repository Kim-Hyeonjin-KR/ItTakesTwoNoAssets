// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NailWeaponComponent.generated.h"

class UInputMappingContext;
class UEnhancedInputLocalPlayerSubsystem;
class USpringArmComponent;
class UInputAction;
class AItTakesTwoCharacter;
struct FInputActionValue;
class ANail;
class UUserWidget;
class UAnimMontage;


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
	void Recall(const FInputActionValue& Value);
	
	void CatchNail(ANail* RecalledNail);
	
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category= "Nail")
	void AddNail();
	
	UFUNCTION(BlueprintCallable, Category = "Nail")
	void RecallTargetNail(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Nail")
	void RecallAllNail();
	
	UFUNCTION(BlueprintCallable, Category= "Nail")
	FVector GetAimLocation();
	
	UFUNCTION(BlueprintCallable, Category= "Nail")
	ANail* FindPinedNail();
	
	UPROPERTY(EditAnywhere, Category= "Nail")
	TSubclassOf<UUserWidget> CrosshairWidgetClass;
	
	UPROPERTY(EditAnywhere, Category= "Nail")
	UUserWidget* CrosshairWidget;
	
	UPROPERTY(EditAnywhere, Category= "Nail")
	UAnimMontage* NailMontage;
	
	void SetInitTargetArmLength(const float _InitTargetArmLength) { InitTargetArmLength = _InitTargetArmLength; }
	void SetInitSocketOffset(const FVector& _InitSocketOffset) { InitSocketOffset = _InitSocketOffset; }
	
protected:
	UPROPERTY()
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem;
	
private:
	TWeakObjectPtr<AItTakesTwoCharacter> OwnerCharacter;
	TWeakObjectPtr<USpringArmComponent> OwnerSpringArm;
	
	float InitTargetArmLength;
	FVector InitSocketOffset;
	
	UPROPERTY(EditAnywhere, Category= "Nail", meta=(AllowPrivateAccess=true))
	float AimingArmLength = 400.f;
	
	UPROPERTY(EditAnywhere, Category= "Nail", meta=(AllowPrivateAccess=true))
	FVector AimingSocketOffset = FVector(0,150,0);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Nail", meta = (AllowPrivateAccess = "true"))
	float CurrentAimBlendValue;
	
	UPROPERTY(BlueprintReadOnly, Category= "Nail", meta=(AllowPrivateAccess=true))
	int MaxNailCount = 3;
	
	UPROPERTY(EditAnywhere, Category= "Nail", meta=(AllowPrivateAccess=true))
	float MaxAimRange = 10000.0f;
	
	UPROPERTY(EditAnywhere, Category= "Nail", meta=(AllowPrivateAccess=true))
	TSubclassOf<ANail> NailClass;
	
	UPROPERTY()
	TArray<ANail*> Nails;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* NailMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ClimbingInput, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ClimbingInput, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ClimbingInput, meta = (AllowPrivateAccess = "true"))
	UInputAction* RecallAction;
	
	bool bHoldingAimButton;
};
