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
	
	if (ItemType == EHandItemType::HoldButton || ItemType == EHandItemType::ToggleButton)
	{
		return;
	}
	else
	{
		//한글은 인코딩 형식 변경 필요.
		FMessageLog("EditorErrors").Warning(FText::FromString("버튼은 HoldButton이나 ToggleButton만 선택해주세요"));
		ItemType = EHandItemType::ToggleButton;
	}
}


// Called when the game starts or when spawned
void AButtonActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AButtonActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

EHandItemType AButtonActor::ActiveItemInteract_Implementation(AActor* Interactor)
{
	if (ItemType == EHandItemType::ToggleButton)
	{
		ToggleButtonActive();
	}
	else if (ItemType == EHandItemType::HoldButton)
	{
		HoldButtonActive();
	}
	return ItemType;
}

void AButtonActor::DeactiveItemInteract_Implementation(AActor* Interactor)
{
	IGrabInterableInterface::DeactiveItemInteract_Implementation(Interactor);
	
	if (ItemType == EHandItemType::HoldButton)
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
	
	SendSignal();
}

void AButtonActor::HoldButtonActive()
{
	bActive = true;
	UE_LOG(LogTemp, Log, TEXT("홀드 버튼 활성화"));
	SendSignal();
}

void AButtonActor::ReleaseHoldButton()
{
	bActive = false;
	UE_LOG(LogTemp, Log, TEXT("홀드 버튼 비활성화"));
	SendSignal();
}

void AButtonActor::SendSignal()
{
	//연결된 전자기기에 신호 전달
	for (const auto& ElectronicActor : LinkedElectronics)
	{
		if (ElectronicActor->Implements<UElectronicInterface>())
		{
			IElectronicInterface::Execute_OnSignal(ElectronicActor, this, bActive);
		}
	}
}