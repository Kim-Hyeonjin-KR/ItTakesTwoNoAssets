// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Nail.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Rendering/RenderCommandPipes.h"

ANail::ANail()
{
	CollisionBox = CreateDefaultSubobject<UBoxComponent>("CapsuleComponent");
	RootComponent = CollisionBox;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMesh");
	SkeletalMesh->SetupAttachment(CollisionBox);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->UpdatedComponent =  RootComponent;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->Activate(false);
}

void ANail::Shoting(FVector TargetLocation)
{
	//ProjectileMovement->Velocity = TargetLocation * ProjectileMovement->InitialSpeed;
	ProjectileMovement->Velocity = FVector(3000.0f, 0.0f, 0.0f);
	ProjectileMovement->Activate(true);
}

void ANail::Pinned(AActor* Target)
{
	ProjectileMovement->Velocity = FVector::ZeroVector;
	ProjectileMovement->Activate(false);
	
	//잡을 수 있는 상태로 변경
}

bool ANail::Recalling()
{
	if (AnimInstance == nullptr) { UE_LOG(LogTemp, Warning, TEXT("Recalling 함수의 AnimInstance is nullptr")); return false; }
	if (PullOutMontage == nullptr) { UE_LOG(LogTemp, Warning, TEXT("Recalling 함수의 PullOutMontage is nullptr")); return false; }
	
	//뽑히기 애니메이션 출력
	
	
	//날아오기
	//부착
	//부착 애니메이션 출력
	//보관중
	
	//뽑히기 애니메이션 출력
	//날아오기
	//아무튼 실패
	//보관중
	
	//부착
	
	return false;
}

void ANail::StoreToNailSocket(USkeletalMeshComponent* NailOwnerMesh)
{
	if (NailOwnerMesh == nullptr) { UE_LOG(LogTemp, Warning, TEXT("StoreToNailSocket 함수의 NailOwnerMesh is nullptr")); return; }
	
	
	AttachToComponent(NailOwnerMesh, FAttachmentTransformRules::KeepRelativeTransform, FName("NailSocket"));
}

void ANail::PlayPullOutAnimation(UAnimMontage* MontageToPlay)
{
	if (MontageToPlay == nullptr) { UE_LOG(LogTemp, Warning, TEXT("Nail 클래스 PlayPullOutAnimation 함수 MontageToPlay 없음")); return; }
	if (SkeletalMesh == nullptr) { UE_LOG(LogTemp, Warning, TEXT("Nail 클래스 PlayPullOutAnimation 함수 SkeletalMesh 없음")); return; }
	if (AnimInstance == nullptr) { UE_LOG(LogTemp, Warning, TEXT("Nail 클래스 PlayPullOutAnimation 함수 SkeletalMesh 없음")); return; }
}

ENailState ANail::GetState() const
{
	return NailState;
}


