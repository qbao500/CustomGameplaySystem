// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/CustomAbilityInfoDataAsset.h"

#include "CustomGameplayTags.h"
#include "GAS/CustomAbilitySystemLibrary.h"
#include "GAS/Abilities/CustomGameplayAbility.h"
#include "UObject/AssetRegistryTagsContext.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomAbilityInfoDataAsset)

FPrimaryAssetId UCustomAbilityInfoDataAsset::GetPrimaryAssetId() const
{
	const FGameplayTag& AbilityTag = UCustomAbilitySystemLibrary::GetFirstAbilityTagFromAbility(Ability.LoadSynchronous());

	if (!AbilityTag.IsValid() || !AbilityTag.MatchesTag(CustomTags::Ability)) return FPrimaryAssetId();
	
	return FPrimaryAssetId(ABILITY_DATA, AbilityTag.GetTagName());
}

const FText& UCustomAbilityInfoDataAsset::GetAbilityDisplayName() const
{
	return AbilityDisplayName;
}

FText UCustomAbilityInfoDataAsset::GetDescriptionAtLevel(const int32 Level) const
{
	if (!Ability) return FText();
	
	FText Description;
	for (const TTuple<int32, FText>& Element : LevelDescriptions)
	{
		if (Level >= Element.Key)
		{
			Description = Element.Value;
		}
	}

	// TODO - This is a temporary solution, will need to load async this
	// Probably make an Async Action to get Ability Description based on Ability Tag
	// It would use Asset Manager to to load with Bundle "Ability" (to load the actual Ability), and then call the below function
	if (UCustomGameplayAbility* AbilityCDO = Ability.LoadSynchronous()->GetDefaultObject<UCustomGameplayAbility>())
	{
		Description = AbilityCDO->FillAbilityDescriptionWithFormat(Description, Level);
	}

	return Description;
}

void UCustomAbilityInfoDataAsset::GetCostAndCooldown(int32 Level, float& OutCost, float& OutCooldown) const
{
	if (!Ability) return;

	if (const UCustomGameplayAbility* AbilityCDO = Ability.LoadSynchronous()->GetDefaultObject<UCustomGameplayAbility>())
	{
		OutCost = AbilityCDO->GetAbilityCost(Level);
		OutCooldown = AbilityCDO->GetAbilityCooldown(Level);
	}
}

void UCustomAbilityInfoDataAsset::GetAssetRegistryTags(FAssetRegistryTagsContext Context) const
{
	Super::GetAssetRegistryTags(Context);

	const FAssetRegistryTag AbilityRegistryTag (GET_MEMBER_NAME_CHECKED(UCustomAbilityInfoDataAsset, PrimaryAbilityTag),
		PrimaryAbilityTag.ToString(), FAssetRegistryTag::TT_Alphabetical);
	Context.AddTag(AbilityRegistryTag);
}

#if WITH_EDITOR
void UCustomAbilityInfoDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FPrimaryAssetId PrimaryAssetId = GetPrimaryAssetId();
	if (PrimaryAssetId.IsValid())
	{
		PrimaryAbilityTag = FGameplayTag::RequestGameplayTag(PrimaryAssetId.PrimaryAssetName, false);
	}

	FillPreviewDescription();
}

void UCustomAbilityInfoDataAsset::PreSave(FObjectPreSaveContext SaveContext)
{
	FillPreviewDescription();
	
	Super::PreSave(SaveContext);
}

void UCustomAbilityInfoDataAsset::FillPreviewDescription()
{
	if (!Ability) return;
	if (UCustomGameplayAbility* AbilityCDO = Ability.LoadSynchronous()->GetDefaultObject<UCustomGameplayAbility>())
	{
		PreviewDescription = AbilityCDO->FillAbilityDescriptionWithFormat(GetDescriptionAtLevel(PreviewLevel), PreviewLevel);
	}
}
#endif
