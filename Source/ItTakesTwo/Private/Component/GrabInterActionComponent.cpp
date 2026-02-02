// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/GrabInterActionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Interface/GrabInterableInterface.h"
#include "DrawDebugHelpers.h"

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
	
	//손에 든 아이템이 없다면, 대상에 따라 상호작용 가능한 상태
	if (CurrentPickUpItemType == EPickUpItemType::None)
	{
		//레이 캐스트를 쏴서 있으면 상호작용 요청
		TryActivateInteractionItem();
		return;
	}
	
	//손에 든 아이템이 있다면, 내려 놓기만 가능
	if (EquipedItem)
	{
		if (CurrentPickUpItemType == EPickUpItemType::Small)
		{
			PutDownItem(EPickUpItemType::Small);

		}
		else if (CurrentPickUpItemType == EPickUpItemType::Heavy)
		{
			PutDownItem(EPickUpItemType::Heavy);
		}
	}
}

void UGrabInterActionComponent::CustomInterActionCancle()
{
	
}

void UGrabInterActionComponent::TryActivateInteractionItem()
{
	UE_LOG(LogTemp, Log, TEXT("TryActivateInteractionItem"));
	
	//박스로 레이를 쏴서 검사하기
	float RaycastLength = CapsuleComponent->GetScaledCapsuleRadius() * 2;
	
	FVector StartForwardLocation = OwnerActor->GetActorLocation();
	FVector EndForwardLocation = StartForwardLocation + (CapsuleComponent->GetForwardVector() * RaycastLength);
	
	FVector BoxHalfSize = FVector(5.0f, CapsuleComponent->GetScaledCapsuleRadius() , CapsuleComponent->GetScaledCapsuleHalfHeight());
	FCollisionShape MyBox = FCollisionShape::MakeBox(BoxHalfSize);
	
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerActor);
	
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		StartForwardLocation,
		EndForwardLocation,
		OwnerActor->GetActorRotation().Quaternion(),
		ECC_Visibility,
		MyBox,
		Params
	);
	
	FVector BoxExtent = FVector(5.0f, CapsuleComponent->GetScaledCapsuleRadius(), CapsuleComponent->GetScaledCapsuleHalfHeight());
	FQuat Rotation = OwnerActor->GetActorRotation().Quaternion();
	
	FColor DebugColor = bHit ? FColor::Green : FColor::Red;

	DrawDebugBox(GetWorld(), StartForwardLocation, BoxExtent, Rotation, DebugColor, false, 2.0f);
	DrawDebugBox(GetWorld(), EndForwardLocation, BoxExtent, Rotation, DebugColor, false, 2.0f);
	DrawDebugLine(GetWorld(), StartForwardLocation, EndForwardLocation, DebugColor, false, 2.0f);

	if (bHit)
	{
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.f, 12, FColor::Yellow, false, 2.0f);
	}
	
	if (HitResult.GetActor() != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("HitResult %s"), *HitResult.GetActor()->GetName());
	}
	
	AActor* HitActor = HitResult.GetActor();
	
	if (HitActor)
	{
		// E키로 상호 작용 가능한 인터페이스가 있다면 실행하고, 객체의 종류를 받아옴.
		if (HitActor->Implements<UGrabInterableInterface>())
		{
			EPickUpItemType ItemType = IGrabInterableInterface::Execute_ActiveItemInteract(HitActor, OwnerActor);
			HandleInteraction(ItemType, HitActor);
		}
	}
}

void UGrabInterActionComponent::HandleInteraction(EPickUpItemType TargetType, AActor* HitActor)
{
	EquipedItem = HitActor;
	
	switch (TargetType)
	{
	case EPickUpItemType::Small:
	case EPickUpItemType::Heavy:
		PickUpItem(TargetType);
		break;
	case EPickUpItemType::HoldButton:
		PushHoldButton();
		break;
	case EPickUpItemType::ToggleButton:
		EquipedItem = nullptr;	//토글 버튼은 일회성 상호작용
		HitToggleButton();
		break;
	case EPickUpItemType::HoldLever:
		HoldLever();
		break;
	case EPickUpItemType::PullableObject:
		PullObject();
		break;
	default:
		break;
	}
}

void UGrabInterActionComponent::PickUpItem(EPickUpItemType TargetType)
{
	if (OnItemPickUp.IsBound())
	{
		OnItemPickUp.Execute(TargetType);
	}
}

void UGrabInterActionComponent::PutDownItem(EPickUpItemType TargetType)
{
	if (EquipedItem->Implements<UGrabInterableInterface>())
	{
		IGrabInterableInterface::Execute_DeactiveItemInteract(EquipedItem, OwnerActor);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PutDownItem 함수. EquipedItem에 UGrabInterableInterface 없음."));
	}
	
	if (OnItemPutDown.IsBound())
	{
		OnItemPutDown.Execute(TargetType);
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PutDownItem 함수. OnItemPutDown에 IsBound가 없음."));
	}
}

void UGrabInterActionComponent::PushHoldButton()
{
	if (OnButtonHold.IsBound())
	{
		OnButtonHold.Execute();
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PushHoldButton 함수. OnButtonHold IsBound가 없음."));
	}
}

void UGrabInterActionComponent::ReleaseHoldButton()
{
	if (OnButtonRelease.IsBound())
	{
		OnButtonRelease.Execute();
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ReleaseHoldButton 함수. OnButtonRelease에 IsBound가 없음."));
	}
}

void UGrabInterActionComponent::HitToggleButton()
{
	if (OnToggleSwitched.IsBound())
	{
		OnToggleSwitched.Execute(HitButtonMontage);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HitToggleButton 함수. OnToggleSwitched에 IsBound가 없음."));
	}
}

void UGrabInterActionComponent::HoldLever()
{
	if (OnLeverPulled.IsBound())
	{
		OnLeverPulled.Execute();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HoldLever 함수. OnLeverPulled 에 IsBound가 없음."));
	}
}

void UGrabInterActionComponent::ReleaseLever()
{
	if (OnLeverReleased.IsBound())
	{
		OnLeverReleased.Execute();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ReleaseLever 함수. OnLeverReleased 에 IsBound가 없음."));
	}
}

void UGrabInterActionComponent::PullObject()
{
	if (OnObjectPull.IsBound())
	{
		OnObjectPull.Execute();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PullObject 함수. OnPushStarted 에 IsBound가 없음."));
	}
}

void UGrabInterActionComponent::ReleasePullObject()
{
	if (OnPullReleased.IsBound())
	{
		OnPullReleased.Execute();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ReleasePullObject 함수. OnPullReleased 에 IsBound가 없음."));
	}
}

