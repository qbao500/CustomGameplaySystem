// Fill out your copyright notice in the Description page of Project Settings.

#include "DataAssets/ExpLootRewardDataAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ExpLootRewardDataAsset)

int32 UExpLootRewardDataAsset::GetRewardXP(const int32 Level) const
{
	return RewardXP.GetValueAtLevel(Level);
}
