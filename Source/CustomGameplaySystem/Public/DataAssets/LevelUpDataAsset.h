// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScalableFloat.h"
#include "Engine/DataAsset.h"
#include "LevelUpDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FExtraLevelInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	int32 FromLevel = 1;
	UPROPERTY(EditDefaultsOnly)
	float DeltaRequiredXP = 100.f;
};

/**
 * 
 */
UCLASS(BlueprintType)
class CUSTOMGAMEPLAYSYSTEM_API ULevelUpDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure)
	int32 CalculateNewLevel(const int32 CurrentLevel, const float NewTotalXP) const;
	
	UFUNCTION(BlueprintPure)
	float GetXPRequiredForLevel(int32 Level) const;
	
	UFUNCTION(BlueprintPure)
	float CalculatePercentageForXPBar(const int32 CurrentLevel, const float CurrentXP) const;

protected:

	// Required to use Curve Table, with each key being a level and value being XP needed for that level
	UPROPERTY(EditDefaultsOnly)
	FScalableFloat ExperienceForLevel;

	// Max level is capped at the last key of the curve used by ExperienceForLevel
	// If false, there is no max level cap. Then, define extra levels in ExtraLevels
	UPROPERTY(EditDefaultsOnly)
	bool bHasMaxLevelCap = true;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "!bHasMaxLevelCap", TitleProperty = "From level {FromLevel} needs {DeltaRequiredXP} XP"))
	TArray<FExtraLevelInfo> ExtraLevels;

	FRealCurve* GetExperienceForLevelCurve() const;
	float GetMaxLevelFromCurve() const;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
