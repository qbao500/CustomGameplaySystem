// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageTypes/CustomDamageType.h"

#include "FunctionLibraries/GameplayTagFunctionLibrary.h"

void UCustomDamageType::ModifyKnockBackForce(float& ForceRef)
{
	if (KnockBackForce > 0.0f)
	{
		ForceRef = KnockBackForce;
	}
}

bool UCustomDamageType::HasTag(const FGameplayTag Tag) const
{
	return DamageTags.HasTag(Tag);
}

bool UCustomDamageType::HasTag(const FName& Tag) const
{
	return DamageTags.HasTag(TAG_FL::MakeTag(Tag));
}
