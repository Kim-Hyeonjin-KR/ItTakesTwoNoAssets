// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/GrabInterActionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Interface/GrabInterableInterface.h"
#include "DrawDebugHelpers.h"
#include "InteractionActor/LeverActor.h"


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
	
	// 몽타주 제대로 넣었는지 확인
	if (ensureAlways(HitButtonMontage) == false)
	{
		UE_LOG(LogTemp,Error, TEXT("HitButtonMontage 비었음"));
	}
	if (ensureAlways(HoldButtonMontage) == false)
	{
		UE_LOG(LogTemp,Error, TEXT("HoldButtonMontage 비었음"));
	}
	if (ensureAlways(HoldLeverMontage) == false)
	{
		UE_LOG(LogTemp,Error, TEXT("HoldLeverMontage 비었음"));
	}
	if (ensureAlways(PullPushMontage) == false)
	{
		UE_LOG(LogTemp,Error, TEXT("PullObjectMontage 비었음"));
	}
	if (ensureAlways(PickUpMontage) == false)
	{
		UE_LOG(LogTemp,Error, TEXT("PickUpMontage 비었음"));
	}
	
	if (ensureAlways(PutDownMontage) == false)
	{
		UE_LOG(LogTemp,Error, TEXT("PutDownMontage 비었음"));
	}
	
	
	
	// ...
	
}


// Called every frame
void UGrabInterActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGrabInterActionComponent::CustomInterAction(const EHandItemType CurrentPickUpItemType)
{
	UE_LOG(LogTemp, Log, TEXT("CustomInterAction"));
	
	//손에 잡힌 아이템이 없다면, 대상에 따라 상호작용 가능한 상태
	if (CurrentPickUpItemType == EHandItemType::None)
	{
		//레이 캐스트를 쏴서 있으면 상호작용 요청
		TryActivateInteractionItem();
		return;
	}
	//손에 무언가 있을 때.
	else
	{
		CustomCancleInterAction(CurrentPickUpItemType);
	}
}

void UGrabInterActionComponent::CustomCancleInterAction(const EHandItemType CurrentPickUpItemType)
{
	//손에 잡힌 아이템이 있는 경우
	if (EquipedItem)
	{
		if (CurrentPickUpItemType == EHandItemType::Small)
		{
			PutDownItem(EHandItemType::Small);
		}
		else if (CurrentPickUpItemType == EHandItemType::Heavy)
		{
			PutDownItem(EHandItemType::Heavy);
		}
		else if (CurrentPickUpItemType == EHandItemType::HoldButton)
		{
			ReleaseHoldButton();
		}
		else if (CurrentPickUpItemType == EHandItemType::PullableObject)
		{
			ReleasePullObject();
		}
	}
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
			EHandItemType ItemType = IGrabInterableInterface::Execute_ActiveItemInteract(HitActor, OwnerActor);
			HandleInteraction(ItemType, HitActor);
		}
	}
}

void UGrabInterActionComponent::HandleInteraction(EHandItemType TargetType, AActor* HitActor)
{
	EquipedItem = HitActor;
	
	switch (TargetType)
	{
	case EHandItemType::Small:
	case EHandItemType::Heavy:
		PickUpItem(TargetType);
		break;
	case EHandItemType::HoldButton:
		PushHoldButton();
		break;
	case EHandItemType::ToggleButton:
		HitToggleButton();
		EquipedItem = nullptr;	//토글 버튼은 일회성 상호작용
		break;
	case EHandItemType::ToggleLever:
		HitLever();
		EquipedItem = nullptr;	//토글 레버도 일회성 상호작용
		break;
	case EHandItemType::PullableObject:
		PullObject();
		break;
	default:
		break;
	}
}

void UGrabInterActionComponent::PickUpItem(EHandItemType TargetType)
{
	if (OnItemPickUp.IsBound())
	{
		OnItemPickUp.Execute(TargetType, PickUpMontage);
	}
}

void UGrabInterActionComponent::PutDownItem(EHandItemType TargetType)
{
	//내려 놓는 애니메이션에 물체의 위치가 정해져 있어서 캐릭터에서 애니메이션 먼저 출력하고 때어내야 함.
	if (OnItemPutDown.IsBound())
	{
		OnItemPutDown.Execute(TargetType, PutDownMontage);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PutDownItem 함수. OnItemPutDown에 IsBound가 없음."));
	}
}

// AN_DetachItem 노티파이로 호출됨.
void UGrabInterActionComponent::DetachItem()
{
	if (EquipedItem == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("DetachItem 함수. EquipedItem에 EquipedItem 없음."));
		return;
	}
	if (EquipedItem->Implements<UGrabInterableInterface>())
	{
		IGrabInterableInterface::Execute_DeactiveItemInteract(EquipedItem, OwnerActor);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PutDownMantageEnd 함수. EquipedItem에 UGrabInterableInterface 없음."));
	}
	
	EquipedItem = nullptr;
}

AActor* UGrabInterActionComponent::GetEquipedItem() const
{
	if (EquipedItem != nullptr)
	{
		return EquipedItem;
	}
	
	return nullptr;
}

/*
void UGrabInterActionComponent::PutDownMantageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), 20.f, 12, FColor::Blue, false, 5.f);
	
	
	//정상종료했으면 문제 없음.
	if (bInterrupted == false)
	{
		return;
	}
	
	//애니메이션 재생이 끝났는데 안떨어졌다면 강제로 때어냄
	if (EquipedItem == nullptr)
	{
		return;
	}
	
	UE_LOG(LogTemp,Warning,TEXT("내려놓기 몽타주가 종료되었지만, 손에서 떨어지지 않았음을 확인. 강제로 떼어냄."));
	DetachItem();
}
*/

void UGrabInterActionComponent::PushHoldButton()
{
	if (OnButtonHold.IsBound())
	{
		if (HoldButtonMontage != nullptr)
		{
			OnButtonHold.Execute(HoldButtonMontage);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PushHoldButton 함수. OnButtonHold IsBound가 없음."));
	}
}

void UGrabInterActionComponent::ReleaseHoldButton()
{
	if (EquipedItem->Implements<UGrabInterableInterface>())
	{
		IGrabInterableInterface::Execute_DeactiveItemInteract(EquipedItem, OwnerActor);
	}
	
	if (OnButtonRelease.IsBound())
	{
		if (HoldButtonMontage != nullptr)
		{
			OnButtonRelease.Execute(HoldButtonMontage);
		}
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
		if (HitButtonMontage != nullptr)
		{
			OnToggleSwitched.Execute(HitButtonMontage);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HitToggleButton 함수. OnToggleSwitched에 IsBound가 없음."));
	}
}

void UGrabInterActionComponent::HitLever()
{
	if (EquipedItem == nullptr)
	{
		return;
	}
	
	ALeverActor* LeverActor = Cast<ALeverActor>(EquipedItem);
	if (LeverActor == nullptr || GetOwner() == nullptr)
	{
		UE_LOG(LogTemp,Warning, TEXT("HitLever 함수.LeverActor 혹은 GetOwner가 없습니다."));
		return;
	}
	
	bool bLeverIsLeft = LeverActor->IsHandleTilteLeft();
	FVector CharToLever = LeverActor->GetActorLocation() - GetOwner()->GetActorLocation();
	
	//float DotResult = FVector::DotProduct(LeverActor->GetActorForwardVector(), CharToLever);
	bool bIsBehing = FVector::DotProduct(LeverActor->GetActorForwardVector(), CharToLever) < 0.0f;
	
	if (bIsBehing)
	{
		//레버의 정면이 아닌, 뒤에 있는 상황이면 방향 반전
		bLeverIsLeft = !bLeverIsLeft;
	}
	
	if (HoldLeverMontage == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("HitLever 함수. HoldLeverMontage가 없음."));
		return;
	}
	
	// IsBound하고 Execute하는게 디버깅에 용이
	if (OnLeverSwitched.IsBound())
	{
		OnLeverSwitched.Execute(HoldLeverMontage, bLeverIsLeft);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HitLever 함수. OnLeverSwitched 에 IsBound가 없음."));
	}
}

void UGrabInterActionComponent::PullObject()
{
	if (OnObjectPull.IsBound())
	{
		if (PullPushMontage != nullptr)
		{
			OnObjectPull.Execute(PullPushMontage);
		}
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
		if (PullPushMontage != nullptr)
		{
			OnPullReleased.Execute(PullPushMontage);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ReleasePullObject 함수. OnPullReleased 에 IsBound가 없음."));
	}
}

