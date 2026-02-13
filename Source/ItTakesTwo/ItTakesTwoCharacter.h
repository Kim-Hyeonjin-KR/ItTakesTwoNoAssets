// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ItTakesTwo/ItTakesTwoTypes.h"
#include "ItTakesTwoCharacter.generated.h"

class UCustomCharacterMovementComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UEnhancedInputLocalPlayerSubsystem;
class UGrabInterActionComponent;

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
class UAnimInstance;
class UItTakesTwoPlayerAnimInstance;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ClimbingInput, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ClimbUpMonta;

	
	/** OnCapsuleHit Event */
	UFUNCTION()
	void OnClimbableWallDetectionOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
								UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
								bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnClimbableWallDetectionEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
								UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
public:
	void SetAnimLayer(EHandItemType Type) const;
	
	//몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* DashMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* GroundPoundMontage;
	
	UFUNCTION(BlueprintCallable)
	void ClimbUpEnded();
	
	UFUNCTION()
	void OnIgnoreInputMontaEnded(UAnimMontage* Montage, bool bInterrupted);
	
	AItTakesTwoCharacter(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dash)
	float DashLength = 100;
	
	UFUNCTION(BlueprintCallable, Category = "MovementState")
	int32 GetCurrentMovementStateFlag();
	
	UFUNCTION(BlueprintPure)
	FVector GetLocalVelocity() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MovementState)
	EMovementState CurrentMovementModeState;
	
	bool bCanDash = true;
	bool bCanJump = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MovementState)
	int CurrentJumpCount;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LockOn)
	bool bIsLockOnMode = false;

	UFUNCTION(BlueprintCallable, Category = LockOn)
	void SetLockOnMode(bool bLockOn);
	
	//GrabInterAction 컴포넌트에 연동할 함수들
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GrabInterAction)
	EHandItemType PickUpItem;
	
	UFUNCTION(BlueprintCallable, Category = GrabInterAction)
	void SetPickUpItemType(EHandItemType Type,  UAnimMontage* PickUpMontage);
	
	UFUNCTION(BlueprintCallable, Category = GrabInterAction)
	void SetPutDownItemType(EHandItemType Type,  UAnimMontage* PutDownMontage);
	
	UFUNCTION(BlueprintCallable, Category = GrabInterAction)
	void ToggleSwitched(UAnimMontage* HitButtonMontage);
	
	UFUNCTION(BlueprintCallable, Category = GrabInterAction)
	void ButtonHold(UAnimMontage* HoldButtonMontage);
	
	UFUNCTION(BlueprintCallable, Category = GrabInterAction)
	void ButtonRelease(UAnimMontage* HoldButtonMontage);
	
	UFUNCTION(BlueprintCallable, Category = GrabInterAction)
	void LeverActive(UAnimMontage* HitLeverMontage, bool bIsLeft = false);
	
	UFUNCTION(BlueprintCallable, Category = GrabInterAction)
	void ObjectPull(UAnimMontage* PullPushMontage);
	
	UFUNCTION(BlueprintCallable, Category = GrabInterAction)
	void PullReleased(UAnimMontage* PullPushMontage);
	
	//이동 상태에 따른 매핑 컨텍스트 변경
	UFUNCTION(BlueprintCallable)
	void SetBaseMovementMappingContext();
	
	//플레이어 방향키 입력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	FVector2D InputMovementVector;
	
	//커스텀 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CustomComponent)
	UGrabInterActionComponent* GrabInterActionComponent;
	
	UFUNCTION(BlueprintCallable)
	void SetInteractionInputLock(bool bInputLock);
	
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	void StopMove();

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	/** Called for Dash input */
	void Dash(const FInputActionValue& Value);

	void CustomJump(const FInputActionValue& Value);
	void CustomStopJumping();
	
	void CustomInterAction(const FInputActionValue& Value);
	
	void CustomCrouch(const FInputActionValue& Value);
	virtual void Landed(const FHitResult& Hit) override;
	
	void Climb(const FInputActionValue& Value);
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	bool GetIgnoreMoveInput() const { return bIgnoreMoveInput; }
	bool GetIgnoreInteractionInput() const { return bIgnoreInteractionInput; }

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCustomCharacterMovementComponent* CustomCharacterMovementComp;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GroundPounnd")
	int MaxGroundPoundCount = 1;
	int CurrentGroundPoundCount = 0;
	
	bool TryGroundPoundBreak(const FHitResult& Hit);
	 
	void TryClimbUp();
	
	void SetIgnoreInputPlayingMontage(UAnimMontage* TargetMontage);
	
	bool CheckLineTrace(FVector StartVec, FVector EndVec);
	
	bool bIgnoreMoveInput;
	bool bIgnoreInteractionInput;
	
	float DefaultCapsuleHalfHeight;
	
	UPROPERTY()
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem;
	
	FVector WallNormal;
	
	UPROPERTY()
	UItTakesTwoPlayerAnimInstance* AnimInst;
	FOnMontageEnded ClimbUpMontageEndedDelegate;
};


