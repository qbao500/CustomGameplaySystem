// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/LevelUpDataAsset.h"

#include "FunctionLibraries/PrintLogFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LevelUpDataAsset)

int32 ULevelUpDataAsset::CalculateNewLevel(const int32 CurrentLevel, const float NewTotalXP) const
{
	const int32 MaxLevelFromCurve = GetMaxLevelFromCurve();
	int32 NewLevel = CurrentLevel;

	while (true)
	{
		// Stop if we reached the max level (when capped)
		if (bHasMaxLevelCap && NewLevel >= MaxLevelFromCurve)
		{
			break;
		}

		const int32 NextLevelXP = GetXPRequiredForLevel(NewLevel + 1);

		if (NewTotalXP >= NextLevelXP)
		{
			++NewLevel;
		}
		else
		{
			break;
		}
	}

	return NewLevel;
}

float ULevelUpDataAsset::GetXPRequiredForLevel(int32 Level) const
{
	const int32 MaxLevelFromCurve = GetMaxLevelFromCurve();

	// Normal case before reaching the max level from the curve
	if (Level <= MaxLevelFromCurve)
	{
		return ExperienceForLevel.GetValueAtLevel(Level);
	}

	// If there's a cap on the level, return the XP needed for the max level
	const float MaxXPFromCurve = ExperienceForLevel.GetValueAtLevel(MaxLevelFromCurve);
	if (bHasMaxLevelCap)
	{
		return MaxXPFromCurve;
	}

	// There's no cap, and level exceed max from curve, then use info from ExtraLevels
	FExtraLevelInfo FoundInfo;
	for (const FExtraLevelInfo& ExtraInfo : ExtraLevels)
	{
		// Find the most accurate info for extra level
		if (Level >= ExtraInfo.FromLevel)
		{
			// Don't break the loop, because we want to find the most accurate info
			// ExtraLevels is sorted by FromLevel
			FoundInfo = ExtraInfo;
		}
	}

	const int32 Multiplier = Level - FoundInfo.FromLevel + 1;
	return MaxXPFromCurve + (FoundInfo.DeltaRequiredXP * Multiplier);
}

float ULevelUpDataAsset::CalculatePercentageForXPBar(const int32 CurrentLevel, const float CurrentXP) const
{
	if (CurrentLevel <= 0 || CurrentXP <= 0) return 0.0f;

	const int32 MaxLevelFromCurve = GetMaxLevelFromCurve();
	if (CurrentLevel >= MaxLevelFromCurve && bHasMaxLevelCap) return 1.0f;

	const int32 MaxXP = GetXPRequiredForLevel(CurrentLevel + 1);
	const int32 MinXP = GetXPRequiredForLevel(CurrentLevel);

	return FMath::GetMappedRangeValueClamped(FVector2D(MinXP, MaxXP), FVector2D(0.0f, 1.0f), CurrentXP);
}

FRealCurve* ULevelUpDataAsset::GetExperienceForLevelCurve() const
{
	return ExperienceForLevel.Curve.GetCurve("ExperienceForLevel");
}

float ULevelUpDataAsset::GetMaxLevelFromCurve() const
{
	const FRealCurve* Curve = GetExperienceForLevelCurve();
	if (!Curve) return 0;

	const int32 MaxLevelFromCurve = Curve->GetKeyTime(Curve->GetLastKeyHandle());
	return MaxLevelFromCurve;
}

void ULevelUpDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(ULevelUpDataAsset, ExtraLevels))
	{
		if (bHasMaxLevelCap) return;
		
		int32 CurrentMaxLevel = GetMaxLevelFromCurve();

		if (ExtraLevels.Num() <= 0)
		{
			ExtraLevels.AddDefaulted();
		}
		
		for (FExtraLevelInfo& ExtraLevel : ExtraLevels)
		{
			if (ExtraLevel.FromLevel <= CurrentMaxLevel)
			{
				CurrentMaxLevel++;
				ExtraLevel.FromLevel = CurrentMaxLevel;
			}
			else
			{
				CurrentMaxLevel = ExtraLevel.FromLevel;
			}
		}

		// Sort the array by FromLevel
		ExtraLevels.Sort([](const FExtraLevelInfo& A, const FExtraLevelInfo& B)
		{
			return A.FromLevel < B.FromLevel;
		});

		return;
	}
}
