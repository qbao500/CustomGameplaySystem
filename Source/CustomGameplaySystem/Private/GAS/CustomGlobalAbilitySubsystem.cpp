// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CustomGlobalAbilitySubsystem.h"

#include "GameplayAbilitySpec.h"
#include "Abilities/GameplayAbility.h"
#include "GAS/Components/CustomAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomGlobalAbilitySubsystem)

void FGlobalAppliedAbilityList::AddToASC(const TSubclassOf<UGameplayAbility>& Ability, UCustomAbilitySystemComponent* ASC)
{
	if (!Ability || !ASC) return;
	
	if (Handles.Contains(ASC))
	{
		RemoveFromASC(ASC);
	}

	UGameplayAbility* AbilityCDO = Ability->GetDefaultObject<UGameplayAbility>();
	const FGameplayAbilitySpec AbilitySpec(AbilityCDO);
	const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
	Handles.Add(ASC, AbilitySpecHandle);
}

void FGlobalAppliedAbilityList::RemoveFromASC(UCustomAbilitySystemComponent* ASC)
{
	if (const FGameplayAbilitySpecHandle* SpecHandle = Handles.Find(ASC))
	{
		ASC->ClearAbility(*SpecHandle);
		Handles.Remove(ASC);
	}
}

void FGlobalAppliedAbilityList::RemoveFromAll()
{
	for (auto& Pair : Handles)
	{
		if (Pair.Key)
		{
			Pair.Key->ClearAbility(Pair.Value);
		}
	}
	Handles.Empty();
}

void FGlobalAppliedEffectList::AddToASC(const TSubclassOf<UGameplayEffect>& Effect, UCustomAbilitySystemComponent* ASC)
{
	if (Handles.Contains(ASC))
	{
		RemoveFromASC(ASC);
	}

	const UGameplayEffect* GameplayEffectCDO = Effect->GetDefaultObject<UGameplayEffect>();
	const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(GameplayEffectCDO, /*Level=*/ 1, ASC->MakeEffectContext());
	Handles.Add(ASC, GameplayEffectHandle);
}

void FGlobalAppliedEffectList::RemoveFromASC(UCustomAbilitySystemComponent* ASC)
{
	if (const FActiveGameplayEffectHandle* EffectHandle = Handles.Find(ASC))
	{
		ASC->RemoveActiveGameplayEffect(*EffectHandle);
		Handles.Remove(ASC);
	}
}

void FGlobalAppliedEffectList::RemoveFromAll()
{
	for (auto& Pair : Handles)
	{
		if (Pair.Key != nullptr)
		{
			Pair.Key->RemoveActiveGameplayEffect(Pair.Value);
		}
	}
	Handles.Empty();
}

void UCustomGlobalAbilitySubsystem::ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability)
{
	if (!Ability || AppliedAbilities.Contains(Ability)) return;

	FGlobalAppliedAbilityList& Entry = AppliedAbilities.Emplace(Ability);
	for (UCustomAbilitySystemComponent* ASC : RegisteredASCs)
	{
		Entry.AddToASC(Ability, ASC);
	}
}

void UCustomGlobalAbilitySubsystem::ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect)
{
	if (!Effect || AppliedEffects.Contains(Effect)) return;

	FGlobalAppliedEffectList& Entry = AppliedEffects.Emplace(Effect);
	for (UCustomAbilitySystemComponent* ASC : RegisteredASCs)
	{
		Entry.AddToASC(Effect, ASC);
	}
}

void UCustomGlobalAbilitySubsystem::RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability)
{
	if (!Ability) return;

	if (FGlobalAppliedAbilityList* Entry = AppliedAbilities.Find(Ability))
	{
		Entry->RemoveFromAll();
		AppliedAbilities.Remove(Ability);
	}
}

void UCustomGlobalAbilitySubsystem::RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect)
{
	if (!Effect) return;

	if (FGlobalAppliedEffectList* Entry = AppliedEffects.Find(Effect))
	{
		Entry->RemoveFromAll();
		AppliedEffects.Remove(Effect);
	}
}

void UCustomGlobalAbilitySubsystem::RegisterASC(UCustomAbilitySystemComponent* ASC)
{
	check(ASC);

	for (auto& Entry : AppliedAbilities)
	{
		Entry.Value.AddToASC(Entry.Key, ASC);
	}
	for (auto& Entry : AppliedEffects)
	{
		Entry.Value.AddToASC(Entry.Key, ASC);
	}

	RegisteredASCs.Emplace(ASC);
}

void UCustomGlobalAbilitySubsystem::UnregisterASC(UCustomAbilitySystemComponent* ASC)
{
	check(ASC);
	
	for (auto& Entry : AppliedAbilities)
	{
		Entry.Value.RemoveFromASC(ASC);
	}
	for (auto& Entry : AppliedEffects)
	{
		Entry.Value.RemoveFromASC(ASC);
	}

	RegisteredASCs.Remove(ASC);
}
