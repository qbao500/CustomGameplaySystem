// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CustomAbilitySystemGlobals.h"
#include "CustomGameplayTags.h"
#include "CustomLogChannels.h"
#include "GAS/Attributes/HealthAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomAbilitySystemGlobals)

static FAutoConsoleCommand CVarDumpAllAttributeTags(
	TEXT("Custom.DumpAllAttributeTags"),
	TEXT("Dumps all attribute tags and their corresponding attributes."),
	FConsoleCommandDelegate::CreateStatic(&UCustomAbilitySystemGlobals::DumpAllAttributeTags)
);

void UCustomAbilitySystemGlobals::InitGlobalTags()
{
	Super::InitGlobalTags();

	// Cache all Damage tags including the parent
	DamageTags = UGameplayTagsManager::Get().RequestGameplayTagChildren(CustomTags::Damage);
	DamageTags.AddTagFast(CustomTags::Damage);

	InitAttributeTags();
}

const FGameplayTagContainer& UCustomAbilitySystemGlobals::GetDamageTags()
{
	return DamageTags;
}

const TMap<FGameplayTag, FGameplayAttribute>& UCustomAbilitySystemGlobals::GetAttributeTagMap()
{
	return AttributeTagMap;
}

const FGameplayAttribute& UCustomAbilitySystemGlobals::GetAttributeFromTag(const FGameplayTag& Tag)
{
	if (const FGameplayAttribute* Attribute = AttributeTagMap.Find(Tag))
	{
		return *Attribute;
	}

	return EmptyAttribute;
}

void UCustomAbilitySystemGlobals::DumpAllAttributeTags()
{
	for (const auto& Pair : AttributeTagMap)
	{
		const FGameplayTag& Tag = Pair.Key;
		const FGameplayAttribute& Attribute = Pair.Value;

		UE_LOG(LogCustom, Warning, TEXT("Tag: %s, Attribute: %s"), *Tag.ToString(), *Attribute.GetName());
	}
}

void UCustomAbilitySystemGlobals::InitAttributeTags()
{
	AttributeTagMap.Emplace(CustomTags::Attribute_Vital_Health, UHealthAttributeSet::GetHealthAttribute());
	AttributeTagMap.Emplace(CustomTags::Attribute_Vital_MaxHealth, UHealthAttributeSet::GetMaxHealthAttribute());
	AttributeTagMap.Emplace(CustomTags::Attribute_Vital_HealthRegen, UHealthAttributeSet::GetHealthRegenerationAttribute());
}
