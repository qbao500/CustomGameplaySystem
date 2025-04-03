// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadSystem/CustomSaveGame.h"
#include "LevelExpSaveGame.generated.h"

/**
 * A CustomSaveGame that is used by LevelExpComponent to save and load level and exp data
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API ULevelExpSaveGame : public UCustomSaveGame
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	int32 Level = 1;

	UPROPERTY()
	int32 Experience = 0;
};
