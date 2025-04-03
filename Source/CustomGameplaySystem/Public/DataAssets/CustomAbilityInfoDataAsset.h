// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "CustomAbilityInfoDataAsset.generated.h"

#define ABILITY_DATA "AbilityData"

class UCustomGameplayAbility;

/**
 * This is a Primary Data Asset that holds a CustomGameplayAbility.
 * PrimaryAssetType is always "AbilityData". PrimaryAssetName is the first tag of the AbilityTags in the Ability.
 * Inherit this class to add extra information about an Ability.
 */
UCLASS(BlueprintType)
class CUSTOMGAMEPLAYSYSTEM_API UCustomAbilityInfoDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AssetBundles = "Ability"))
	TSoftClassPtr<UCustomGameplayAbility> Ability = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Input"))
	FGameplayTag StartingInputTag = FGameplayTag();

	UFUNCTION(BlueprintPure)
	const FText& GetAbilityDisplayName() const;

	// This will load synchronously the Ability
	// If this DataAsset is loaded with Asset Manager, you can add "Ability" Bundle to async load it first
	UFUNCTION(BlueprintPure)
	FText GetDescriptionAtLevel(int32 Level) const;
	UFUNCTION(BlueprintPure)
	void GetCostAndCooldown(int32 Level, float& OutCost, float& OutCooldown) const;

	virtual void GetAssetRegistryTags(FAssetRegistryTagsContext Context) const override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Description")
	FText AbilityDisplayName = FText();

	// Not needed for every level, just declare the levels that need a new description, and it will apply to next levels.
	// For ex, if you have 5 levels, and you only declare 1, 3, 5, then the description will be the same for 2 (as 1) and 4 (as 3). Level 6 and so on will be the same as 5.
	// Level must be in ascending order.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (MultiLine = true), Category = "Description")
	TMap<int32, FText> LevelDescriptions;

	// This is the First Ability Tag of the AbilityTags in the Ability.
	// This is also the PrimaryAssetName of PrimaryAssetId.
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "PrimaryAssetId", meta = (DisplayAfter = "PrimaryAssetType"))
	FGameplayTag PrimaryAbilityTag = FGameplayTag::EmptyTag;

private:

#if WITH_EDITORONLY_DATA
	// Add this value of PrimaryAssetType to Asset Manager project settings.
	// This specific variable doesn't do anything, just here in editor to show the PrimaryAssetType.
	UPROPERTY(VisibleDefaultsOnly, Category = "PrimaryAssetId")
	FName PrimaryAssetType = FName(ABILITY_DATA);

	UPROPERTY(EditDefaultsOnly, Category = "Description|Preview")
	int32 PreviewLevel = 1;
	UPROPERTY(VisibleDefaultsOnly, meta = (MultiLine = true), Category = "Description|Preview")
	FText PreviewDescription = FText();
#endif

public:
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
	void FillPreviewDescription();
#endif
};
