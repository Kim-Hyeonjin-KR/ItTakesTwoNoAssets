// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItTakesTwo/ItTakesTwoTypes.h"
#include "GrabInterActionComponent.generated.h"

class UCapsuleComponent;

DECLARE_DELEGATE_OneParam(FOnItemPickUpSuccess , EPickUpItemType)
DECLARE_DELEGATE_OneParam(FOnItemPutDownSuccess , EPickUpItemType)


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
	FOnItemPickUpSuccess OnItemPickUp;
	FOnItemPutDownSuccess OnItemPutDown;
	
private:
	void TryActivateInteractionItem();
	
	void PickUpItem(EPickUpItemType TargetType);
	void PutDownItem(EPickUpItemType TargetType);
	void PushHoldButton();
	void ReleaseHoldButton();
	void HitToggleButton();
	void HoldLever();
	void ReleaseLever();
	
private:
	AActor* OwnerActor;
	UCapsuleComponent * CapsuleComponent;
	AActor* EquipedItem;
};
