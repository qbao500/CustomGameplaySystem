// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemGlobals.h"
#include "CustomAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UCustomAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

public:

	virtual void InitGlobalTags() override;

	static const FGameplayTagContainer& GetDamageTags();
	static const TMap<FGameplayTag, FGameplayAttribute>& GetAttributeTagMap();
	static const FGameplayAttribute& GetAttributeFromTag(const FGameplayTag& Tag);

	static void DumpAllAttributeTags();

protected:

	// Override this to add more attribute tags to AttributeTagMap
	virtual void InitAttributeTags();

	// Cached Damage tags including parent, so it can be used anywhere
	inline static FGameplayTagContainer DamageTags;

	// Override InitAttributeTags to add more attribute tags
	inline static TMap<FGameplayTag, FGameplayAttribute> AttributeTagMap;

private:

	inline static FGameplayAttribute EmptyAttribute;
};
