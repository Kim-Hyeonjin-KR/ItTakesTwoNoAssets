// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/HammerWeaponComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Component/NailWeaponComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ItTakesTwo/ItTakesTwoCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon/Hammer.h"

// Sets default values for this component's properties
UHammerWeaponComponent::UHammerWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UHammerWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	
	AddHammer();
	
	OwnerCharacter = Cast<AItTakesTwoCharacter>(GetOwner());
	
	ensure(OwnerCharacter.Get());
}


// Called every frame
void UHammerWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHammerWeaponComponent::SetupHammerActionInput(class UInputComponent* PlayerInputComponent)
{
	if (OwnerCharacter.Get() == nullptr)
	{
		OwnerCharacter = Cast<AItTakesTwoCharacter>(GetOwner());
		ensure(OwnerCharacter.Get());
	};
	ensureMsgf(OwnerCharacter.IsValid(), TEXT("UHammerWeaponComponent %s가 null 입니다."), *GetName());
	
	if (APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController()))
	{
		EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (EnhancedInputSubsystem != nullptr)
		{
			EnhancedInputSubsystem->AddMappingContext(HammerMappingContext, WeaponPriority);
		}
	}
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// SpecialAbility 못 공격, 조준
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &UHammerWeaponComponent::Attack);
	}
	
	
}

void UHammerWeaponComponent::SetHammerCollision(bool bCollisionOn)
{
	if (Hammer == nullptr) { UE_LOG(LogTemp, Warning, TEXT("UHammerWeaponComponent 의 SetHammerCollision 함수. Hammer 이 없습니다")); return; }
	if (Hammer->HeadCollision == nullptr) { UE_LOG(LogTemp, Warning, TEXT("UHammerWeaponComponent 의 SetHammerCollision 함수. HeadCollision 이 없습니다")); return; }
	
	if (bCollisionOn)
	{
		Hammer->HeadCollision->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	}
	else
	{
		Hammer->HeadCollision->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
}

void UHammerWeaponComponent::Attack()
{
	UE_LOG(LogTemp,Warning,TEXT("헤머 어택"));
	
	if (OwnerCharacter.IsValid() == false) { UE_LOG(LogTemp, Warning, TEXT("HammerWeaponComponent의 OwnerCharacter이 없습니다.")) return;}
	if (OwnerCharacter->GetIgnoreMoveInput() || OwnerCharacter->GetIgnoreInteractionInput()) { return; }
	
	UCharacterMovementComponent* CharacterMovementComponent = Cast<UCharacterMovementComponent>(OwnerCharacter->GetCharacterMovement());
	if (CharacterMovementComponent == nullptr) { return; }
	if (OwnerCharacter->GetCharacterMovement()->MovementMode != MOVE_Walking) { return; }
	if (HammerAttackMontage == nullptr) { UE_LOG(LogTemp,Warning,TEXT("HammerAttackMontage가 없습니다.")) return;}
	
	OwnerCharacter->PlayAnimMontage(HammerAttackMontage);
}


void UHammerWeaponComponent::AddHammer()
{
	Hammer = GetWorld()->SpawnActor<AHammer>(HammerClass, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
	Hammer->SetOwner(GetOwner());
	Hammer->SetHammerOwnerCharacter(GetOwner());
	Hammer->AttachHammer("NailSocket");
}

void UHammerWeaponComponent::ChangeSocket(FName TargetSocketName)
{
	Hammer->AttachHammer(TargetSocketName);
}

void UHammerWeaponComponent::SetCollisionActive(bool bCollisionSet)
{
	if (bCollisionSet)
	{
		Hammer->HeadCollision->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	}
	else
	{
		Hammer->HeadCollision->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
}
