// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScalableFloat.h"
#include "Engine/DataAsset.h"
#include "LevelUpDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class CUSTOMGAMEPLAYSYSTEM_API ULevelUpDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure)
	int32 FindLevelForXP(const int32 XP) const;

	// For example, if Level 2 needs 200 XP and Level 3 needs 300 XP, this function will return 100
	UFUNCTION(BlueprintPure)
	int32 FindDeltaXPNeededForLevel(const int32 Level) const;

	// For example, if Level 2 needs 200 XP, this function will return 200
	UFUNCTION(BlueprintPure)
	int32 FindTotalXPNeededForLevel(const int32 Level) const;

	UFUNCTION(BlueprintPure)
	float CalculatePercentageForXPBar(const int32 CurrentLevel, const int32 CurrentXP) const;

protected:

	// Required to use Curve Table, with each key being a level and value being XP needed for that level
	UPROPERTY(EditDefaultsOnly)
	FScalableFloat ExperienceForLevel;

	FRealCurve* GetExperienceForLevelCurve() const;
};
