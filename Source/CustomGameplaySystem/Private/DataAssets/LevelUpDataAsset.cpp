// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/LevelUpDataAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LevelUpDataAsset)

int32 ULevelUpDataAsset::FindLevelForXP(const int32 XP) const
{
	int32 Level = 1;
	
	const FRealCurve* Curve = GetExperienceForLevelCurve();
	if (!Curve) return Level;

	// Loop through all keys
	TArray<FKeyHandle>::TConstIterator Iterator = Curve->GetKeyHandleIterator();
	while (Iterator)
	{
		const FKeyHandle KeyHandle = *Iterator;
		const float Value = Curve->GetKeyValue(KeyHandle);
		if (XP >= Value)
		{
			Level = Curve->GetKeyTime(KeyHandle);
		}
		++Iterator;
	}

	return Level;
}

int32 ULevelUpDataAsset::FindDeltaXPNeededForLevel(const int32 Level) const
{
	if (Level <= 1) return 0;

	const int32 MaxXP = FindTotalXPNeededForLevel(Level);
	const int32 MinXP = FindTotalXPNeededForLevel(Level - 1);

	return FMath::Abs(MaxXP - MinXP);
}

int32 ULevelUpDataAsset::FindTotalXPNeededForLevel(const int32 Level) const
{
	if (Level <= 1) return 0;

	const FRealCurve* Curve = GetExperienceForLevelCurve();
	if (!Curve) return 0;

	return Curve->Eval(Level);
}

float ULevelUpDataAsset::CalculatePercentageForXPBar(const int32 CurrentLevel, const int32 CurrentXP) const
{
	if (CurrentLevel <= 0 || CurrentXP <= 0) return 0.0f;

	const FRealCurve* Curve = GetExperienceForLevelCurve();
	if (!Curve) return 0.0f;

	const int32 MaxLevel = Curve->GetNumKeys();
	if (CurrentLevel >= MaxLevel) return 1.0f;

	const int32 MaxXP = FindTotalXPNeededForLevel(CurrentLevel + 1);
	const int32 MinXP = FindTotalXPNeededForLevel(CurrentLevel);

	return FMath::GetMappedRangeValueClamped(FVector2D(MinXP, MaxXP), FVector2D(0.0f, 1.0f), CurrentXP);
}

FRealCurve* ULevelUpDataAsset::GetExperienceForLevelCurve() const
{
	return ExperienceForLevel.Curve.GetCurve("ExperienceForLevel");
}
