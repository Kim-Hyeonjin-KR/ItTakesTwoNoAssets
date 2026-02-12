// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "Nail.generated.h"

class ANail;
DECLARE_DELEGATE_OneParam(FOnRecallEnd, ANail*);

class UBoxComponent;
class UProjectileMovementComponent;
class UAnimMontage;
class AItTakesTwoCharacter;



/**
 * 
 */

UENUM(BlueprintType)
enum class ENailState : uint8 
{
	Stored UMETA(DisplayName = "Stored"),
	Ready UMETA(DisplayName = "Ready"),
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
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Skeletal Mesh")
	USkeletalMeshComponent* NailSkeletalMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectile")
	UProjectileMovementComponent* ProjectileMovement;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
	UBoxComponent* CollisionBox;
	
	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
							  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
							  bool bFromSweep, const FHitResult& SweepResult);
	
	FOnRecallEnd OnRecallEnd;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void Shoting(FVector TargetLocation);
	
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void Pinned(UPrimitiveComponent* OtherComp, const FHitResult& SweepResult);
	
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void Recalling();
	
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void Store();
	
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void Ready();
	
	UFUNCTION(BlueprintPure)
	bool IsMoving() const;
	
	UFUNCTION(BlueprintPure)
	bool HasNailOwner() const;
	
	// 몽타주 모음
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	UAnimMontage* EquipMontage;
	
	void PlayMontage(UAnimMontage* MontageToPlay, FName SectionName = FName()) const;

	bool IsGrabable() const;
	
	ENailState GetState() const;
	
	void SetNailOwnerCharacter(AActor* OwnerCharacter);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	ENailState NailState;
	
	FVector StoreSocketOffset;
	
protected:
	FTimerHandle RecallTimerHandle;
	void OnTimeOutRecall();
	
private:
	UPROPERTY()
	TWeakObjectPtr<AItTakesTwoCharacter> NailOwner;
	
	//자기 자신에서 사용하니까 강한 참조
	UPROPERTY()
	TObjectPtr<UAnimInstance> AnimInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed", meta = (AllowPrivateAccess))
	float RecallSpeed = 12000.f;
};
