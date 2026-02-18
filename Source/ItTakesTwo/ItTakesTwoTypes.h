// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItTakesTwoTypes.generated.h"

static constexpr int8 MoveMappingPriority = 5;
static constexpr int8 SwingPriority = 7;
static constexpr int8 WeaponPriority = 8;


UENUM(BlueprintType)
enum class EHandItemType : uint8
{
	None UMETA(DisplayName = "None"),
	Small UMETA(DisplayName = "Small"),
	Heavy UMETA(DisplayName = "Heavy"),
	ToggleButton UMETA(DisplayName = "ToggleButton"),
	HoldButton UMETA(DisplayName = "HoldButton"),
	ToggleLever UMETA(DisplayName = "ToggleLever"),
	PullableObject UMETA(DisplayName = "PullableObject"),
	Nail UMETA(DisplayName = "Nail"),
	Hammer UMETA(DisplayName = "Hammer"),
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