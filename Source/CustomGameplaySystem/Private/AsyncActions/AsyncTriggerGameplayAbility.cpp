// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncActions/AsyncTriggerGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "GAS/Abilities/CustomGameplayAbility.h"

UAsyncTriggerGameplayAbility* UAsyncTriggerGameplayAbility::AsyncTriggerAbilityByClass(
	UAbilitySystemComponent* AbilitySystemComponent, TSubclassOf<UGameplayAbility> AbilityClass)
{
	// Register with the game instance to avoid being garbage collected
	UAsyncTriggerGameplayAbility* Task = NewObject<UAsyncTriggerGameplayAbility>();
	Task->RegisterWithGameInstance(AbilitySystemComponent);
	
	if (!IsValid(AbilitySystemComponent))
	{
		PLFL::PrintLog("Couldn't create Task, missing ASC");
		Task->bWillFail = true;
		return Task;
	}

	if (!AbilityClass)
	{
		PLFL::PrintLog("No Ability Class provided!");
		Task->bWillFail = true;
		return Task;
	}
	
	const FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass);
	if (AbilitySpec == nullptr)
	{
		PLFL::PrintLog("Couldn't create Task, Ability " + AbilityClass->GetName() + " not found in . Please add this in Default Abilities");
		Task->bWillFail = true;
		return Task;
	}

	UGameplayAbility* AbilityInstance = AbilitySpec->GetPrimaryInstance();
	if (AbilityInstance == nullptr)
	{
		PLFL::PrintLog("Couldn't create Task, Ability " + AbilityClass->GetName() + " couldn't find an instance. Instance Policy shouldn't be 'instance per execution'");
		Task->bWillFail = true;
		return Task;
	}
	
	Task->bWillFail = false;
	Task->DelegateHandle = AbilityInstance->OnGameplayAbilityEnded.AddUObject(Task, &UAsyncTriggerGameplayAbility::OnAbilityEnded);
	Task->AbilityListeningTo = AbilityInstance;
	AbilitySystemComponent->TryActivateAbility(AbilitySpec->Handle);
	
	return Task;
}

UAsyncTriggerGameplayAbility* UAsyncTriggerGameplayAbility::AsyncTriggerAbilityByTag(
	UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTagContainer& Tags)
{
	// Register with the game instance to avoid being garbage collected
	UAsyncTriggerGameplayAbility* Task = NewObject<UAsyncTriggerGameplayAbility>();
	Task->RegisterWithGameInstance(AbilitySystemComponent);
	
	if (!IsValid(AbilitySystemComponent))
	{
		PLFL::PrintLog("Couldn't create Task, missing ASC");
		Task->bWillFail = true;
		return Task;
	}

	if (Tags.IsEmpty() || !Tags.IsValid())
	{
		PLFL::PrintLog("Couldn't create Task, tags are empty or not valid");
		Task->bWillFail = true;
		return Task;
	}

	// Only find the 1st
	TArray<FGameplayAbilitySpec*> AbilitySpecs;
	AbilitySystemComponent->GetActivatableGameplayAbilitySpecsByAllMatchingTags(Tags, AbilitySpecs);
	if (!AbilitySpecs.IsValidIndex(0) || !AbilitySpecs[0])
	{
		PLFL::PrintLog("No valid ability found");
		Task->bWillFail = true;
		return Task;
	}

	const FGameplayAbilitySpec* AbilitySpec = AbilitySpecs[0];
	UGameplayAbility* AbilityInstance = AbilitySpec->GetPrimaryInstance();
	if (AbilityInstance == nullptr)
	{
		PLFL::PrintLog("Couldn't create Task, Ability " + Tags.ToString() + " couldn't find an instance. Instance Policy shouldn't be 'instance per execution'");
		Task->bWillFail = true;
		return Task;
	}

	Task->bWillFail = false;
	Task->DelegateHandle = AbilityInstance->OnGameplayAbilityEnded.AddUObject(Task, &UAsyncTriggerGameplayAbility::OnAbilityEnded);
	Task->AbilityListeningTo = AbilityInstance;
	AbilitySystemComponent->TryActivateAbility(AbilitySpec->Handle);
	
	return Task;
}

void UAsyncTriggerGameplayAbility::EndTask()
{
	if (AbilityListeningTo)
	{
		AbilityListeningTo->OnGameplayAbilityEnded.Remove(DelegateHandle);
		AbilityListeningTo->K2_CancelAbility();
	}
	
	SetReadyToDestroy();
	MarkAsGarbage();
}

void UAsyncTriggerGameplayAbility::OnAbilityEnded(UGameplayAbility* Ability)
{
	//PLFL::PrintWarning("OnAbilityEnded");

	OnFinished.Broadcast();
	
	if (AbilityListeningTo)
	{
		AbilityListeningTo->OnGameplayAbilityEnded.Remove(DelegateHandle);
	}
	
	SetReadyToDestroy();
}

void UAsyncTriggerGameplayAbility::OnAbilityFailed()
{
	OnFailed.Broadcast();

	SetReadyToDestroy();
}

void UAsyncTriggerGameplayAbility::Activate()
{
	Super::Activate();
	
	if (bWillFail)
	{
		OnAbilityFailed();
	}
}

