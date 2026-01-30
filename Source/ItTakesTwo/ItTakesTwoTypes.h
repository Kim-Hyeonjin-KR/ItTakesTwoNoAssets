// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItTakesTwoTypes.generated.h"



UENUM(BlueprintType)
enum class EPickUpItemType : uint8
{
	None UMETA(DisplayName = "None"),
	Small UMETA(DisplayName = "Small"),
	Heavy UMETA(DisplayName = "Heavy")
};

UENUM(BlueprintType)
enum class EInterActionItemType : uint8
{
	None			UMETA(DisplayName = "None"),
	PushButton		UMETA(DisplayName = "PushButton"),
	ToggleButton	UMETA(DisplayName = "ToggleButton"),
	Lever			UMETA(DisplayName = "Lever"),
	GrabableItem	UMETA(DisplayName = "GrabableItem"),
};