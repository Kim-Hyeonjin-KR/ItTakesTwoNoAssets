// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/GrabInterActionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Interface/GrabInterableInterface.h"

// Sets default values for this component's properties
UGrabInterActionComponent::UGrabInterActionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UGrabInterActionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerActor = GetOwner();
	if (OwnerActor == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("그랩 컴포넌트 OwnerActor is nullptr"));
		return;
	}
	CapsuleComponent = OwnerActor->FindComponentByClass<UCapsuleComponent>();
	if (CapsuleComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("그랩 컴포넌트의 OwnerActor에 캡슐 컴포넌트 is nullptr"))
	}
	
	// ...
	
}


// Called every frame
void UGrabInterActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGrabInterActionComponent::CustomInterAction(const EPickUpItemType CurrentPickUpItemType)
{
	UE_LOG(LogTemp, Log, TEXT("CustomInterAction"));
	
	if (CurrentPickUpItemType == EPickUpItemType::None)
	{
		//레이 캐스트를 쏴서 있으면 상호작용 요청
		TryActivateInteractionItem();
		return;
	}
	
	if (EquipedItem)
	{
		if (EquipedItem->Implements<UGrabInterableInterface>())
		{
			IGrabInterableInterface::Execute_PutDownItemInteract(EquipedItem, OwnerActor);
		}
		if (CurrentPickUpItemType == EPickUpItemType::Small)
		{
			if (OnItemPutDown.IsBound())
			{
				OnItemPutDown.Execute(EPickUpItemType::Small);
				return;
			}
		}
		else if (CurrentPickUpItemType == EPickUpItemType::Heavy)
		{
			if (OnItemPutDown.IsBound())
			{
				OnItemPutDown.Execute(EPickUpItemType::Heavy);
				return;
			}
		}
	}
}

void UGrabInterActionComponent::TryActivateInteractionItem()
{
	UE_LOG(LogTemp, Log, TEXT("TryActivateInteractionItem"));
	
	float RaycastLength = CapsuleComponent->GetScaledCapsuleRadius() * 5;
	
	FVector StartForwardLocation = OwnerActor->GetActorLocation();
	FVector EndForwardLocation = StartForwardLocation + (CapsuleComponent->GetForwardVector() * RaycastLength);
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerActor);
	GetWorld()->LineTraceSingleByChannel(HitResult, StartForwardLocation, EndForwardLocation, ECC_Visibility, Params);
	
	DrawDebugLine(GetWorld(), StartForwardLocation, EndForwardLocation, FColor::Blue, false, 10.0f);
	
	if (HitResult.GetActor() != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("HitResult %s"), *HitResult.GetActor()->GetName());
	}
	
	
	AActor* HitActor = HitResult.GetActor();
	
	if (HitActor)
	{
		
		//잡을 수 있는 인터페이스라면 손에 들기. UGrabInterableInterface pickupable로 바꿔줄것.
		//Implements는 c++ 코드를 기준으로 검사하기 때문에 에디터에서 넣었다면 모른다. 그래서 블루프린트에서 추가한 컴포넌트를 제거하고, 인터페이스를 상속받은 엑터를 부모 클래스로 설정해서 에디터에서 다시 만들어줬다.
		if (HitActor->Implements<UGrabInterableInterface>())
		{
			EPickUpItemType ItemType = IGrabInterableInterface::Execute_PickUpItemInteract(HitActor, OwnerActor);
				
			if (OnItemPickUp.IsBound())
			{
				OnItemPickUp.Execute(ItemType);
			}
			EquipedItem = HitActor;
		}
		//버튼, 레버 추가해줄것
		
		//아래는 제대로 작동하지 않았던 코드. FindComponentByInterface는 블루프린트에서 나중에 넣은 것에 대해서는 잘 찾지 못한다. 계층 구조 차이 때문인데, c++를 먼저 뒤져보고 없어서 블프를 뒤져봤지만, 막상 거기서 찾아도 c++을 기준으로 판단해서 없다고 생각한다고 한다. 
		/*
		UActorComponent* InterfaceComponent = HitActor->FindComponentByInterface(UGrabInterableInterface::StaticClass());
		if (InterfaceComponent != nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("그랩 인터페이스있음 %s"), *HitResult.GetActor()->GetName());
			IGrabInterableInterface::Execute_GrabInteract(InterfaceComponent, OwnerActor);
		}
		else
		{
		UE_LOG(LogTemp, Log, TEXT("그랩 인터페이스 없음 %s"), *HitResult.GetActor()->GetName());
		}
		*/
		
		/*
		//보유 컴포넌트 모두 찾는 방법
		for (UActorComponent* Comp : HitActor->GetComponents())
		{
			UE_LOG(LogTemp, Log, TEXT("보유 컴포넌트: %s"), *Comp->GetName());
		}
		*/
	}
	
}

void UGrabInterActionComponent::PickUpItem(EPickUpItemType TargetType)
{
	
}

void UGrabInterActionComponent::PutDownItem(EPickUpItemType TargetType)
{
	
}

void UGrabInterActionComponent::PushHoldButton()
{
	
}

void UGrabInterActionComponent::ReleaseHoldButton()
{
	
}

void UGrabInterActionComponent::HitToggleButton()
{
	
}

void UGrabInterActionComponent::HoldLever()
{
	
}

void UGrabInterActionComponent::ReleaseLever()
{
	
}

