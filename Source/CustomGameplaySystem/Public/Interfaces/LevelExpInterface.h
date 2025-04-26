// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"
#include "LevelExpInterface.generated.h"

class ULevelExpComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULevelExpInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CUSTOMGAMEPLAYSYSTEM_API ILevelExpInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Level Exp")
	ULevelExpComponent* GetLevelExpComponent() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Level Exp")
	int32 GetCurrentLevel() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Level Exp")
	int32 GetCurrentXP() const;
};
