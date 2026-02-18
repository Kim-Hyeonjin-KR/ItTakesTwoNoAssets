// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HammerWeaponComponent.generated.h"

class ANail;
struct FInputActionValue;
class UInputAction;
class AItTakesTwoCharacter;
class UInputMappingContext;
class UEnhancedInputLocalPlayerSubsystem;
class AHammer;

//블루프린트에서 처리할 내용
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrabNailDelegate, AItTakesTwoCharacter*, HammerOwnerCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndGrabNailDelegate);


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
	
	UPROPERTY(EditAnywhere, Category= "Hammer")
	UAnimMontage* HammerSwingMontage;
	
	UFUNCTION(BlueprintCallable, Category = "Hammer")
	void Attack();
	
	UFUNCTION(BlueprintCallable, Category = "Hammer")
	void SwingMove(const FInputActionValue& Value);
	
	UFUNCTION(BlueprintCallable, Category = "Hammer")
	void SwingJump(const FInputActionValue& Value);
	
	UFUNCTION(BlueprintCallable, Category = "Hammer")
	void AddHammer();
	
	UFUNCTION(BlueprintCallable, Category = "Hammer")
	void ChangeSocket(FName TargetSocketName);
	
	UFUNCTION(BlueprintCallable, Category = "Hammer")
	void SetSwingMappingContext(bool bActive);
	
	UFUNCTION(BlueprintCallable, Category = "Hammer")
	void GrabNail(ANail* TargetNail, bool bRightSide);
	
	UPROPERTY(BlueprintAssignable, Category = "Hammer")
	FOnGrabNailDelegate OnGrabNailDelegate;
	
	UFUNCTION(BlueprintCallable, Category = "Hammer")
	void EndGrabNail();
	
	UPROPERTY(BlueprintAssignable, Category = "Hammer")
	FOnEndGrabNailDelegate OnEndGrabNailDelegate;
	
	UFUNCTION(BlueprintCallable, Category = "Hammer")
	bool IsSwing() const { return bSwing; }
	
private:
	TWeakObjectPtr<AItTakesTwoCharacter> OwnerCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* HammerMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* SwingMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwingMoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwingJumpAction;
	
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void SetCollisionActive(bool bCollisionSet);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bSwing;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bRightSideStart;
	
	TWeakObjectPtr<ANail> SwingTargetNail;
	
	UPROPERTY()
	AHammer* Hammer;
	
	//스윙 계산에 사용될 변수들
	//스윙 구현 포기해서 안쓰임...
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float SwingSpeed = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AngleLimit = 60.f;
	
	//못이 박힌 시점을 기준으로 정방향 최대 회전, 역방향 최대 회전 가능값
	float MaxAngle;
	float MinAngle;
	
	float CurrentRoll;
	float SwingDirection = 1.f;
	FRotator InitRotation;
	
	FVector2D SwingVector;
	

};
