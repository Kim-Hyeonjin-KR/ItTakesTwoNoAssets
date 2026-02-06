// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "Nail.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;
class USkeletalMeshComponent;
class UAnimMontage;

/**
 * 
 */

UENUM(BlueprintType)
enum class ENailState : uint8 
{
	Stored UMETA(DisplayName = "Stored"),
	Flying UMETA(DisplayName = "Flying"),
	Pinned UMETA(DisplayName = "Pinned"),
	Recalling UMETA(DisplayName = "Recalling"),
};

UCLASS()
class ITTAKESTWO_API ANail : public AWeaponBase
{
	GENERATED_BODY()
	
public:
	ANail();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Skeletal Mesh")
	USkeletalMeshComponent* SkeletalMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectile")
	UProjectileMovementComponent* ProjectileMovement;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
	UBoxComponent* CollisionBox;
	
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void Shoting(FVector TargetLocation);
	
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void Pinned(AActor* Target);
	
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	bool Recalling();
	
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void StoreToNailSocket(USkeletalMeshComponent* NailOwnerMesh);
	
	// 몽타주 모음
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	UAnimMontage* PullOutMontage;
	
public:
	void PlayPullOutAnimation(UAnimMontage* MontageToPlay);
	
	ENailState GetState() const;

private:
	//자기 자신에서 사용하니까 강한 참조
	UPROPERTY()
	TObjectPtr<UAnimInstance> AnimInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State", meta = (AllowPrivateAccess))
	ENailState NailState;
};
