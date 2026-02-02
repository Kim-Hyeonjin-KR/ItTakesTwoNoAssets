// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItTakesTwo/ItTakesTwoTypes.h"
#include "GrabInterActionComponent.generated.h"

class UCapsuleComponent;

DECLARE_DELEGATE_OneParam(FOnGrabbed, EPickUpItemType)
DECLARE_DELEGATE_OneParam(FOnDropped, EPickUpItemType)

DECLARE_DELEGATE_OneParam(FOnToggleSwitched, UAnimMontage*)

DECLARE_DELEGATE(FOnButtonHold)
DECLARE_DELEGATE(FOnButtonRelease)

DECLARE_DELEGATE(FOnLeverPulled)
DECLARE_DELEGATE(FOnLeverReleased)

DECLARE_DELEGATE(FOnObjectPull)
DECLARE_DELEGATE(FOnPullReleased)


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
	void CustomInterAction(const EPickUpItemType CurrentPickUpItemType);
	void CustomInterActionCancle();
	
	//델리게이트
	FOnGrabbed OnItemPickUp;
	FOnDropped OnItemPutDown;
	
	FOnToggleSwitched OnToggleSwitched;
	FOnButtonHold OnButtonHold;
	FOnButtonRelease OnButtonRelease;
	
	FOnLeverPulled OnLeverPulled;
	FOnLeverReleased OnLeverReleased;
	
	FOnObjectPull OnObjectPull;
	FOnPullReleased OnPullReleased;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* HitButtonMontage;
	
	
private:
	void TryActivateInteractionItem();
	void HandleInteraction(EPickUpItemType TargetType, AActor* HitActor);
	
	void PickUpItem(EPickUpItemType TargetType);
	void PutDownItem(EPickUpItemType TargetType);
	void PushHoldButton();
	void ReleaseHoldButton();
	void HitToggleButton();
	void HoldLever();
	void ReleaseLever();
	void PullObject();
	void ReleasePullObject();
	
private:
	AActor* OwnerActor;
	UCapsuleComponent * CapsuleComponent;
	AActor* EquipedItem;
};
