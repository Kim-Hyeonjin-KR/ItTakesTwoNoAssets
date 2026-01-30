// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItTakesTwo/ItTakesTwoTypes.h"
#include "GrabInterableInterface.generated.h"

// This class does not need to be modified.
// UINTERFACE(MinimalAPI, NotBlueprintable) // BlueprintCallable로 만들면 블프에서 가져다 쓸 때, 구현부가 있어야 한다. 
											// 그런데 순수 가상 함수로 만들어버리면 구현부가 없어서 애초에 블프에서도 가져가지 못하게 막아야 한다.
											// 순수 가상함수를 blueprint callable로 만든다는건 모순이다. 그냥 억지로 블프에서 만들 수 없도록 막아서 해결해버린셈.

UINTERFACE(MinimalAPI)
class UGrabInterableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ITTAKESTWO_API IGrabInterableInterface
{
	GENERATED_BODY()
	
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//UFUNCTION(BlueprintCallable)	// 블루프린트에서 호출만 수 있는 함수임을 선언. 구현부가 반드시 c++에 있어야 한다.
									// 그래서 함수도 c++ 형식으로 사용됨.
	//virtual EPickUpItemType GrabInteract(AActor* Interactor) {return EPickUpItemType::None;};
	
	UFUNCTION(BlueprintNativeEvent)	// 블루프린트와 c++ 모두 수정이 가능한 함수임을 선언. 반대로 말하면 이미 virtual이기 때문에 virtual을 또 붙이면 에러!
	EPickUpItemType PickUpItemInteract(AActor* Interactor);
	
	UFUNCTION(BlueprintNativeEvent)
	void PutDownItemInteract(AActor* Interactor);
};
