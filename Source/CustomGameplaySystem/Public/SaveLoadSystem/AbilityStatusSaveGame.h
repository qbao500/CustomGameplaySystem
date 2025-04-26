// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "GAS/Components/AbilityStatusComponent.h"
#include "SaveLoadSystem/CustomSaveGame.h"
#include "AbilityStatusSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UAbilityStatusSaveGame : public UCustomSaveGame
{
	GENERATED_BODY()

public:

	const TMap<FGameplayTag, FAbilityStatus>& GetAbilityStatusMap() const
	{
		return AbilityStatusMap;
	}

	void SetAbilityStatusMap(const TMap<FGameplayTag, FAbilityStatus>& InAbilityStatusMap)
	{
		AbilityStatusMap = InAbilityStatusMap;
	}

protected:

	UPROPERTY()
	TMap<FGameplayTag, FAbilityStatus> AbilityStatusMap;
};
