// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "Hammer.generated.h"

class AItTakesTwoCharacter;
class UCapsuleComponent;
/**
 * 
 */
UCLASS()
class ITTAKESTWO_API AHammer : public AWeaponBase
{
	GENERATED_BODY()
	
public:
	AHammer();
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(EditAnywhere, Category= "SkeletalMesh")
	USkeletalMeshComponent* HeadSkeletalMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
	UCapsuleComponent* HeadCollision;
	
	UFUNCTION(BlueprintCallable, Category = "Hammer")
	void AttachHammer(FName SocketName = NAME_None);
	
	UFUNCTION(BlueprintCallable, Category = "Hammer")
	bool SetHammerOwnerCharacter(AActor* OwnerCharacter);
	
	UFUNCTION()
	void OnHammerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
private:
	UPROPERTY()
	TWeakObjectPtr<AItTakesTwoCharacter> HammerOwner;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hammer", meta = (AllowPrivateAccess = true))
	FVector LocationOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hammer", meta = (AllowPrivateAccess = true))
	FRotator RotationOffset;
};
