// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScalableFloat.h"
#include "Engine/DataAsset.h"
#include "ExpLootRewardDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UExpLootRewardDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	int32 GetRewardXP(const int32 Level) const;

protected:

	UPROPERTY(EditDefaultsOnly)
	FScalableFloat RewardXP;

private:

	friend class UExpLootDropComponent;
};
