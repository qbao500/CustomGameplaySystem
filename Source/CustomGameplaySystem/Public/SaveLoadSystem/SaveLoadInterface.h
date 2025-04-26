// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"
#include "SaveLoadInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USaveLoadInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CUSTOMGAMEPLAYSYSTEM_API ISaveLoadInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnActorLoadedFromSave();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool ShouldLoadTransform() const;
};
