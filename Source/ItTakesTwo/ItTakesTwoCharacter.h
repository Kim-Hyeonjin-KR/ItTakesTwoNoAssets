// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ItTakesTwoCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UEnhancedInputLocalPlayerSubsystem;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EMovementState : uint8
{
	None		= 0,
	Crouch		= 1 << 0,
	Walk		= 1 << 1,
	Dash	    = 1 << 2,
	Sprint		= 1 << 3,
	Jump		= 1 << 4,
	Climbing	= 1 << 5,
	Flying		= 1 << 6,
	Swimming	= 1 << 7,
};
ENUM_CLASS_FLAGS(EMovementState)

/*
UENUM()
enum ECharacterActionType : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Dash UMETA(DisplayName = "Dash")
};
*/

class UAnimMontage;

UCLASS(config=Game)
class AItTakesTwoCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Climbing, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* ClimbingMappingContext;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;
	
	/** InterAction Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InterAction;
	
	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;
	
	/** Climbing Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ClimbingInput, meta = (AllowPrivateAccess = "true"))
	UInputAction* ClimbingAction;
	
	
	/** OnCapsuleHit Event */
	UFUNCTION()
	void OnClimbableWallDetectionOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
								UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
								bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnClimbableWallDetectionEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
								UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* DashMontage;
	
	AItTakesTwoCharacter(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dash)
	float DashLength = 100;
	
	UFUNCTION(BlueprintCallable, Category = "MovementState")
	int32 GetCurrentMovementStateFlag();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MovementState)
	EMovementState CurrentMovementModeState;
	
	bool bCanDash = true;
	bool bCanJump = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LockOn)
	bool bIsLockOnMode = false;

	//상태에 따른 매핑 컨텍스트 변경
	void SetMappingContext();
	
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	/** Called for Dash input */
	void Dash(const FInputActionValue& Value);

	void CustomJump(const FInputActionValue& Value);
	void CustomStopJumping();
	
	void CustomInterAction(const FInputActionValue& Value);
	
	void CustomCrouch(const FInputActionValue& Value);
	
	void Climb(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
private:
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem;
	FVector WallNormal;
};


