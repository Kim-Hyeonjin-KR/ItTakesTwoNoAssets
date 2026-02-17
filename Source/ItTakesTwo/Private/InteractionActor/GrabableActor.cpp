// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionActor/GrabableActor.h"
#include "ItTakesTwo/ItTakesTwoCharacter.h"

// Sets default values
AGrabableActor::AGrabableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// 피직스 계산 하게 켜두자.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGrabableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrabableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

EHandItemType AGrabableActor::ActiveItemInteract_Implementation(AActor* Interactor)
{
	AttachItem(Interactor);
	
	return ItemType;
}

void AGrabableActor::DeactiveItemInteract_Implementation(AActor* Interactor)
{
	DetachItem(Interactor);
}

void AGrabableActor::ActiveItem_Implementation()
{
	
}

void AGrabableActor::DeActiveItem_Implementation()
{
	
}

void AGrabableActor::AttachItem(AActor* Target)
{
	USkeletalMeshComponent* Mesh = GetTargetMesh(Target);
	
	UE_LOG(LogTemp, Log, TEXT("아이템 타입 %s"), *UEnum::GetValueAsString(ItemType));
	
	if (Mesh == nullptr)
	{
		return;
	}
	
	if (Mesh->DoesSocketExist(FName("Align")) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttachSocket에서 Align 소켓을 찾지 못함"));
		return;
	}

	//물건의 피직스와 콜리전 끄기 
	UPrimitiveComponent* PrimitiveRoot = Cast<UPrimitiveComponent>(GetRootComponent());
	if (PrimitiveRoot != nullptr)
	{
		PrimitiveRoot->SetSimulatePhysics(false);
		PrimitiveRoot->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		this->ActiveItem();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PrimitiveRoot를 찾지 못함"));
		return;
	}
	
	
	this->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Align"));
}


void AGrabableActor::DetachItem(AActor* Target)
{
	USkeletalMeshComponent* Mesh = GetTargetMesh(Target);
	
	if (Mesh == nullptr)
	{
		return;
	}
	
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	DetachFromActor(DetachRules);
	DrawDebugSphere(GetWorld(), GetActorLocation(), 20.f, 12, FColor::Red, false, 5.f);
	
	UPrimitiveComponent* PrimitiveRoot = Cast<UPrimitiveComponent>(GetRootComponent());
	if (PrimitiveRoot != nullptr)
	{
		PrimitiveRoot->SetWorldRotation(FRotator(0.0f, 0.0f, 0.0f));
		PrimitiveRoot->SetSimulatePhysics(true);
		PrimitiveRoot->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
		this->DeActiveItem();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PrimitiveRoot를 찾지 못함"));
		return;
	}
	
}

USkeletalMeshComponent* AGrabableActor::GetTargetMesh(AActor* Target)
{
	AItTakesTwoCharacter* Player = Cast<AItTakesTwoCharacter>(Target);
	if (Player == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttachSocket에서 플레이어를 찾지 못함"));
		return nullptr;
	}
	
	return Player->GetMesh();
}


