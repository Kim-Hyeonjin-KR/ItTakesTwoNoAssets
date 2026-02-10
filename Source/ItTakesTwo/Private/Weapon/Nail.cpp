// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Nail.h"

#include "Component/NailWeaponComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
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

void ANail::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ANail::ANail::HandleOverlap);
	//NailOwner = GetWorld()->GetFirstPlayerController()->GetCharacter();
}

void ANail::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (GetOwner() == nullptr) { UE_LOG(LogTemp,Warning,TEXT("Nail의 Owner이 존재하지 않습니다.")); return; }
	if (GetOwner()->FindComponentByClass<UNailWeaponComponent>() == nullptr){return;}
	
	
	
	if (NailState == ENailState::Recalling)
	{
		FVector TargetLocation = NailOwner->GetMesh()->GetBoneLocation("NailCatchSocket");
		
		//제곱근 대신 제곱 사용
		if (FVector::DistSquared(GetActorLocation(), TargetLocation) < FMath::Square(20.f))
		{
			SetActorLocation(TargetLocation, false);
			this->AttachToComponent(NailOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("NailCatchSocket"));
			ProjectileMovement->Activate(false);
			NailState = ENailState::Stored;
		}
		else
		{
			FVector NewLocation = FMath::VInterpConstantTo(GetActorLocation(), TargetLocation, DeltaSeconds, RecallSpeed);
			SetActorLocation(NewLocation, false);
			UE_LOG(LogTemp,Warning,TEXT("%s"), *NewLocation.ToString());
		}
	}
}


void ANail::HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                          int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr || (OtherActor->IsA(ANail::StaticClass()))) { return; }
	
	if (OtherActor->FindComponentByClass<UNailWeaponComponent>())
	{
		// 회수
		return;
	}

	// 부착 가능한 엑터인지 확인
	if (OtherComp->ComponentHasTag("Pinable"))
	{
		//부착
		Pinned(OtherComp, SweepResult);
		
		return;
	}
	else
	{
		ProjectileMovement->StopMovementImmediately();
		UE_LOG(LogTemp,Warning,TEXT("그냥 박음"));
		// 데미지 적용
	}
	
}

void ANail::Shoting(FVector TargetLocation)
{
	GetWorldTimerManager().ClearTimer(RecallTimerHandle);
	
	this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	NailState = ENailState::Flying;
	
	FRotator LookAtRotation =  UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);
	SetActorRotation(LookAtRotation);
	
	ProjectileMovement->StopMovementImmediately();
	
		//지형지물 벽에 그냥 막히는 경우, 내부적으로 ProjectileMovement가 Block 판정을 띄우고 시뮬레이션을 정지시킵니다.
		//이런 상황에서 Recall로 위치를 강제 이동 시켜도 여전히 벽에 박혀 있는 상황이라는 데이터가 남아 있기 때문에 강제로 초기화 시켜줘야 합니다.
	ProjectileMovement->SetUpdatedComponent(RootComponent);
	
	ProjectileMovement->Activate(true);
	ProjectileMovement->SetVelocityInLocalSpace(FVector(6000.0f, 0.0f, 0.0f));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	
	if (UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(RootComponent))
	{
		RootPrim->WakeAllRigidBodies();
	}
	
	//일정 시간동안 박히지 않으면 자동 회수
	GetWorldTimerManager().SetTimer(RecallTimerHandle, this, &ANail::OnTimeOutRecall, 5.0f, false);
}


void ANail::Pinned(UPrimitiveComponent* OtherComp, const FHitResult& SweepResult)
{
	ProjectileMovement->Velocity = FVector::ZeroVector;
	ProjectileMovement->Activate(false);
	
	//벽에 맞춰 회전시키고 부착
	SetActorRotation(FRotationMatrix::MakeFromX(SweepResult.ImpactNormal * -1).Rotator());
	this->AttachToComponent(OtherComp, FAttachmentTransformRules::KeepWorldTransform);
	
	//잡을 수 있는 상태로 변경
	NailState = ENailState::Pinned;
}

void ANail::Recalling()
{
	//뽑히기
	if (NailState == ENailState::Pinned)
	{
		this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
	ProjectileMovement->Velocity = FVector::ZeroVector;
	
	
	//if (AnimInstance == nullptr) { UE_LOG(LogTemp, Warning, TEXT("Recalling 함수의 AnimInstance is nullptr")); return; }
	NailState = ENailState::Recalling;
	

	//날아오기 (이동은 Tick에서 실행)
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMovement->Activate(true);
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

bool ANail::IsGrabable() const
{
	if (NailState == ENailState::Pinned)
	{
		return true;
	}
	
	return false;
}

ENailState ANail::GetState() const
{
	return NailState;
}

void ANail::SetNailOwnerCharacter(AActor* OwnerCharacter)
{
	ACharacter* CastedOwnerChar =  Cast<ACharacter>(OwnerCharacter);
	
	if (CastedOwnerChar)
	{
		NailOwner = CastedOwnerChar;
	} 
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Nail.cpp의 SetNailOwnerCharacter함수 CastedOwnerChar가 캐스팅에 실패하여 nullptr입니다."));
		UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
	}
}

void ANail::OnTimeOutRecall()
{
	if (NailState == ENailState::Flying || NailState == ENailState::Pinned)
	{
		Recalling();
	}
	
}


