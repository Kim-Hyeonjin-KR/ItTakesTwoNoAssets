// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionActor/ButtonActor.h"

#include "InteractionActor/ElectronicActor.h"
#include "Interface/ElectronicInterface.h"

// Sets default values
AButtonActor::AButtonActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void AButtonActor::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (ItemType == EPickUpItemType::HoldButton || ItemType == EPickUpItemType::ToggleButton)
	{
		return;
	}
	else
	{
		//한글은 인코딩 형식 변경 필요.
		FMessageLog("EditorErrors").Warning(FText::FromString("버튼은 HoldButton이나 ToggleButton만 선택해주세요"));
		ItemType = EPickUpItemType::ToggleButton;
	}
}


// Called when the game starts or when spawned
void AButtonActor::BeginPlay()
{
	Super::BeginPlay();
	
	//시작시, 연결된 전자기기에 신호 전달하여 Map에 등록.
	for (const auto& ElectronicActor : LinkedElectronics)
	{
		if (ElectronicActor == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("ButtonActor에 ElectronicActor이 null 입니다."));
			return;
		}
		if (ElectronicActor->Implements<UElectronicInterface>())
		{
			IElectronicInterface::Execute_OnSignal(ElectronicActor, this, bActive);
		}
	}
}

// Called every frame
void AButtonActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

EPickUpItemType AButtonActor::ActiveItemInteract_Implementation(AActor* Interactor)
{
	if (ItemType == EPickUpItemType::ToggleButton)
	{
		ToggleButtonActive();
	}
	else if (ItemType == EPickUpItemType::HoldButton)
	{
		HoldButtonActive();
	}
	return ItemType;
}

void AButtonActor::DeactiveItemInteract_Implementation(AActor* Interactor)
{
	IGrabInterableInterface::DeactiveItemInteract_Implementation(Interactor);
	
	if (ItemType == EPickUpItemType::HoldButton)
	{
		ReleaseHoldButton();
	}
}

void AButtonActor::ToggleButtonActive()
{
	UE_LOG(LogTemp, Log, TEXT("ToggleButtonActive"));
	
	if (bActive)
	{
		bActive = false;
		UE_LOG(LogTemp, Log, TEXT("버튼 비활성화"));
	}
	else
	{
		bActive = true;
		UE_LOG(LogTemp, Log, TEXT("버튼 활성화"));
	}
	
	//연결된 전자기기에 신호 전달
	for (const auto& ElectronicActor : LinkedElectronics)
	{
		if (ElectronicActor->Implements<UElectronicInterface>())
		{
			IElectronicInterface::Execute_OnSignal(ElectronicActor, this, bActive);
		}
	}
}

void AButtonActor::HoldButtonActive()
{
	UE_LOG(LogTemp, Log, TEXT("HoldButtonActive"));
}

void AButtonActor::ReleaseHoldButton()
{
	UE_LOG(LogTemp, Log, TEXT("ReleaseHoldButton"));
}

