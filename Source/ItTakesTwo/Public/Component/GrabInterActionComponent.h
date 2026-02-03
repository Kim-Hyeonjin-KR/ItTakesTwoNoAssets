// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItTakesTwo/ItTakesTwoTypes.h"
#include "GrabInterActionComponent.generated.h"

class UCapsuleComponent;

DECLARE_DELEGATE_OneParam(FOnGrabbed, EHandItemType)
DECLARE_DELEGATE_OneParam(FOnDropped, EHandItemType)

DECLARE_DELEGATE_OneParam(FOnToggleSwitched, UAnimMontage*)

DECLARE_DELEGATE_OneParam(FOnButtonHold, UAnimMontage*)
DECLARE_DELEGATE_OneParam(FOnButtonRelease, UAnimMontage*)

DECLARE_DELEGATE_TwoParams(FOnLeverSwitched, UAnimMontage*, bool)

DECLARE_DELEGATE_OneParam(FOnObjectPull, UAnimMontage*)
DECLARE_DELEGATE_OneParam(FOnPullReleased, UAnimMontage*)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ITTAKESTWO_API UGrabInterActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabInterActionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:
	void CustomInterAction(const EHandItemType CurrentPickUpItemType);
	void CustomCancleInterAction(const EHandItemType CurrentPickUpItemType);
	
	//델리게이트
	FOnGrabbed OnItemPickUp;
	FOnDropped OnItemPutDown;
	
	FOnToggleSwitched OnToggleSwitched;
	FOnButtonHold OnButtonHold;
	FOnButtonRelease OnButtonRelease;
	
	FOnLeverSwitched OnLeverSwitched;
	
	FOnObjectPull OnObjectPull;
	FOnPullReleased OnPullReleased;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* HitButtonMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* HoldButtonMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* HoldLeverMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* PullPushMontage;
	
private:
	void TryActivateInteractionItem();
	void HandleInteraction(EHandItemType TargetType, AActor* HitActor);
	
	void PickUpItem(EHandItemType TargetType);
	void PutDownItem(EHandItemType TargetType);
	void PushHoldButton();
	void ReleaseHoldButton();
	void HitToggleButton();
	void HitLever();
	void PullObject();
	void ReleasePullObject();
	
private:
	AActor* OwnerActor;
	UCapsuleComponent * CapsuleComponent;
	AActor* EquipedItem;
};
