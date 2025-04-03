// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncActions/WaitForAbilityCooldownChange.h"
#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WaitForAbilityCooldownChange)

UWaitForAbilityCooldownChange* UWaitForAbilityCooldownChange::WaitForAbilityCooldownChange(UAbilitySystemComponent* ASC, const FGameplayTag& InCooldownTag)
{
	if (!IsValid(ASC) || !InCooldownTag.IsValid())
	{
		return nullptr;
	}

	UWaitForAbilityCooldownChange* Node = NewObject<UWaitForAbilityCooldownChange>();
	Node->AbilitySystemComponent = ASC;
	Node->CooldownTag = InCooldownTag;

	ASC->RegisterGameplayTagEvent(InCooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(Node, &UWaitForAbilityCooldownChange::CooldownTagChanged);

	// Register to not get Garbage Collection while running
	Node->RegisterWithGameInstance(ASC);

	return Node;
}

void UWaitForAbilityCooldownChange::Cancel()
{
	Super::Cancel();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	}
}

void UWaitForAbilityCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		OnCooldownEnd.Broadcast(0.0f);
	}
	else
	{
		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InCooldownTag.GetSingleTagContainer());
		TArray<float> Durations = AbilitySystemComponent->GetActiveEffectsTimeRemaining(Query);
		if (Durations.Num() > 0)
		{
			Durations.Sort();
			const float TimeRemaining = Durations[Durations.Num() - 1];
			OnCooldownStart.Broadcast(TimeRemaining);
		}
	}
}