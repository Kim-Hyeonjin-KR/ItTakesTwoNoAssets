// Copyright Epic Games, Inc. All Rights Reserved.

#include "ItTakesTwoCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "ItTakesTwo/Public/Character/CustomCharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AItTakesTwoCharacter


void AItTakesTwoCharacter::OnClimbableWallDetectionOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherActor->ActorHasTag(TEXT("Climbable")))
	{
		UE_LOG(LogTemp,Warning,TEXT("벽에 붙음"));
		
		WallNormal = SweepResult.ImpactNormal;
		CharacterState |= ECharacterState::Climbing;
		SetMappingContext();
	}
	
}

void AItTakesTwoCharacter::OnClimbableWallDetectionEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp,Warning,TEXT("벽에서 떨어짐"));
	CharacterState &= ~ECharacterState::Climbing;
	SetMappingContext();
}

AItTakesTwoCharacter::AItTakesTwoCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AItTakesTwoCharacter::BeginPlay()
{
	// Call the base class
	Super::BeginPlay();
	
	if (GetCapsuleComponent() != nullptr)
	{
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AItTakesTwoCharacter::OnClimbableWallDetectionOverlap);
		GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AItTakesTwoCharacter::OnClimbableWallDetectionEnd);
	}
	
}

//////////////////////////////////////////////////////////////////////////
// Input

void AItTakesTwoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (EnhancedInputSubsystem != nullptr)
		{
			EnhancedInputSubsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AItTakesTwoCharacter::CustomJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AItTakesTwoCharacter::CustomStopJumping);
		
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AItTakesTwoCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AItTakesTwoCharacter::Look);
		
		// Dash
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &AItTakesTwoCharacter::Dash);
		
		// Climbing
		EnhancedInputComponent->BindAction(ClimbingAction, ETriggerEvent::Triggered, this, &AItTakesTwoCharacter::Climb);
		
		// InterAction
		EnhancedInputComponent->BindAction(InterAction, ETriggerEvent::Triggered, this, &AItTakesTwoCharacter::CustomInterAction);
		
		// Crouch
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AItTakesTwoCharacter::CustomCrouch);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AItTakesTwoCharacter::SetMappingContext()
{
	EnhancedInputSubsystem->ClearAllMappings();
	
	if (EnumHasAnyFlags(CharacterState,ECharacterState::Climbing))
	{
		//벽타기 매핑 컨텍스트 추가
		EnhancedInputSubsystem->AddMappingContext(ClimbingMappingContext, 0);
		GetCharacterMovement()->SetMovementMode(MOVE_Custom);
		GetCharacterMovement()->GravityScale = 0.0f;
	}
	else if (EnumHasAnyFlags(CharacterState,ECharacterState::Flying))
	{
		//비행 매핑 컨텍스트 추가
	}
	else if (EnumHasAnyFlags(CharacterState,ECharacterState::Swimming))
	{
		//수영 매핑 컨텍스트 추가
	}
	else
	{
		//기본 매핑 컨텍스트
		EnhancedInputSubsystem->AddMappingContext(DefaultMappingContext, 0);
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		GetCharacterMovement()->GravityScale = 1.0f;
	}
}

void AItTakesTwoCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
		
		
	}
}

void AItTakesTwoCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AItTakesTwoCharacter::Dash(const FInputActionValue& Value)
{
	if (bCanDash && DashMontage)
	{
		PlayAnimMontage(DashMontage);
		
		/*
		FVector CurrentLocation = GetActorLocation();
		FRotator CurrentRotation = Controller->GetControlRotation();
		CurrentRotation.Roll = 0;
		CurrentRotation.Pitch = 0;
		FVector HorizontalForwardVector = FRotationMatrix(CurrentRotation).GetUnitAxis(EAxis::X);
		
		FVector TargetLocation = CurrentLocation + HorizontalForwardVector * DashLength;
		*/
		
		/*
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(DashMontage);
		}
		*/
	}
	
	else
	{
		UE_LOG(LogTemp,Warning, TEXT("Can't Dash"));
	}
	
	UE_LOG(LogTemp,Warning, TEXT("Dash value is now %f"), (GetActorForwardVector() * 1000).Length());
}

void AItTakesTwoCharacter::CustomJump(const FInputActionValue& Value)
{
	UE_LOG(LogTemp,Warning, TEXT("%s"),bCanJump ? TEXT("true") : TEXT("false"));
	if (bCanJump)
	{
		Super::Jump();
	}
}

void AItTakesTwoCharacter::CustomStopJumping()
{
	
	Super::StopJumping();
}

void AItTakesTwoCharacter::CustomInterAction(const FInputActionValue& Value)
{
	UE_LOG(LogTemp,Warning,TEXT("CustomInteraction"));
	
	//잡기
	//누르기
	//들기

	//이벤트 보드로 캐스트해서 인터페이스 있는지 확인. 인터페이스는 3종
	
	
	
}

void AItTakesTwoCharacter::CustomCrouch(const FInputActionValue& Value)
{
	UE_LOG(LogTemp,Warning,TEXT("CustomCrouch"));
	
}

void AItTakesTwoCharacter::Climb(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		FVector UpVector = GetActorUpVector();
		
		FVector WallRight = FVector::CrossProduct(WallNormal, UpVector).GetSafeNormal();
		FVector WallUp = FVector::CrossProduct(WallRight, WallNormal).GetSafeNormal();
		
		// add movement 
		AddMovementInput(WallUp, MovementVector.Y);
		AddMovementInput(WallRight, MovementVector.X);
		
		float CharacterRadius, CharacterHalfHeight;
		
		GetCapsuleComponent()->GetScaledCapsuleSize(CharacterRadius, CharacterHalfHeight);
		FVector ForwardCheckStart = GetActorLocation() + (UpVector * CharacterHalfHeight + 5);
		FVector ForwardCheckEnd = ForwardCheckStart + (GetActorForwardVector() * CharacterRadius * 2);
		FHitResult ForwardHit;
		FCollisionQueryParams Params;
		
		if (false == GetWorld()->LineTraceSingleByChannel(ForwardHit, ForwardCheckStart, ForwardCheckEnd, ECC_Visibility, Params))
		{
			DrawDebugLine(GetWorld(), ForwardCheckStart, ForwardCheckEnd, FColor::Blue, false, 10.0f);
			
			FHitResult DownHit;
			
			FVector HeightCheckStart = ForwardCheckEnd - (GetActorUpVector() * 10);
			FVector HeightCheckEnd = ForwardCheckEnd + (GetActorUpVector() * CharacterRadius * 2);
			if (false == GetWorld()->LineTraceSingleByChannel(DownHit, HeightCheckStart, HeightCheckEnd, ECC_Visibility, Params))
			{
				DrawDebugLine(GetWorld(), HeightCheckStart, HeightCheckEnd, FColor::Green, false, 10.0f);
				
				UE_LOG(LogTemp,Warning, TEXT("올라가기 동작 실행 가능"));
				//Play 올라가기 몽타
				//SetActorLocation(ForwardCheckEnd);
			}
		}
		
	}
}
