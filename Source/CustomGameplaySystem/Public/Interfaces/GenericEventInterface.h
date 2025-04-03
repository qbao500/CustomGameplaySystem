// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GenericEventInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGenericEventInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CUSTOMGAMEPLAYSYSTEM_API IGenericEventInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Generic Event")
	void StartGenericEvent();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Generic Event")
	void EndGenericEvent();
	
};
