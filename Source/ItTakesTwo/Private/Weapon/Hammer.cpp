// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Hammer.h"

#include "Components/CapsuleComponent.h"
#include "Interface/ElectronicInterface.h"
#include "ItTakesTwo/ItTakesTwoCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

AHammer::AHammer()
{
	//머리 메쉬
	HeadSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Head SkeletalMesh");
	RootComponent = HeadSkeletalMesh;
	
	//충돌 판정 컴포넌트. 머리에 부착
	HeadCollision = CreateDefaultSubobject<UCapsuleComponent>("CapsuleComponent");
	HeadCollision->SetupAttachment(HeadSkeletalMesh);
}

void AHammer::BeginPlay()
{
	Super::BeginPlay();
	
	if (HeadCollision)
	{
		HeadCollision->OnComponentBeginOverlap.AddDynamic(this, &AHammer::OnHammerOverlap);
	}
	
}

void AHammer::AttachHammer(FName SocketName)
{
	HeadCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->AttachToComponent(HammerOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	
	if (SocketName != "RightHand")
	{
		//this->AddActorLocalOffset(LocationOffset);
		//this->AddActorLocalRotation(RotationOffset);
		
		FVector WeaponLocation = LocationOffset;
		FRotator WeaponRotation = RotationOffset; // Yaw 90도
		FTransform Transform(WeaponRotation, WeaponLocation, FVector(1.f));
		
		this->SetActorRelativeTransform(Transform);
	}
	else if(SocketName == "RightHand")
	{
		FVector WeaponLocation = FVector(15.f, 0.f, 0.f);
		FRotator WeaponRotation = FRotator(0.f, 90.f, 0.f); // Yaw 90도
		FTransform Transform(WeaponRotation, WeaponLocation, FVector(1.f));
		
		this->SetActorRelativeTransform(Transform);
	}
}

bool AHammer::SetHammerOwnerCharacter(AActor* OwnerCharacter)
{
	AItTakesTwoCharacter* CastedOwnerChar = Cast<AItTakesTwoCharacter>(OwnerCharacter);

	if (CastedOwnerChar)
	{
		HammerOwner = CastedOwnerChar;
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Hammer.cpp의 SetHammerOwnerCharacter함수 CastedOwnerChar가 캐스팅에 실패하여 nullptr입니다."));
		UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit,
		                               false);
		return false;
	}
}

void AHammer::OnHammerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp,Warning,TEXT("오버렙"));
	
	if (OtherActor->ActorHasTag("Hammer"))
	{
		if (OtherActor->Implements<UElectronicInterface>())
		{
			IElectronicInterface::Execute_OnSignal(OtherActor, this, true);
		}
	}
}
