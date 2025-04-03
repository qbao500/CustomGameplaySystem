// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/DeathGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "CustomGameplayTags.h"
#include "Character/CustomHealthComponent.h"

UDeathGameplayAbility::UDeathGameplayAbility()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	// Define ability trigger event, which is triggered from CustomHealthComponent
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		// Add the ability trigger tag as default to the CDO.
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = CustomTags::Event_Character_Death;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UDeathGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	check(ActorInfo);
	check(TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	FGameplayTagContainer AbilityTypesToIgnore;
	AbilityTypesToIgnore.AddTag(CustomTags::Ability_Behavior_SurvivesDeath);
	
	// Cancel all abilities and block others from starting.
	ASC->CancelAbilities(nullptr, &AbilityTypesToIgnore, this);

	SetCanBeCanceled(false);

	if (bAutoStartDeath)
	{
		StartDeath(TriggerEventData->Instigator);
	}
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UDeathGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	check(ActorInfo);

	// Always try to finish the death when the ability ends in case the ability doesn't.
	// This won't do anything if the death hasn't been started.
	FinishDeath();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDeathGameplayAbility::StartDeath(const AActor* DeathInstigator)
{
	if (UCustomHealthComponent* HealthComponent = UCustomHealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == EDeathState::NotDead)
		{
			HealthComponent->StartDeath(DeathInstigator);
		}
	}
}

void UDeathGameplayAbility::FinishDeath()
{
	if (UCustomHealthComponent* HealthComponent = UCustomHealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == EDeathState::DeathStarted)
		{
			HealthComponent->FinishDeath();
		}
	}
}
