// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilityTasks/AbilityTask_CreateTargetData.h"

#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AbilityTask_CreateTargetData)

void UAbilityTask_CreateTargetData::Activate()
{
	Super::Activate();

	if (IsLocallyControlled())
	{
		SendTargetDataLocally();
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey PredictionKey = GetActivationPredictionKey();

		// We are on the Server, so listen for target Data
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, PredictionKey).
			AddUObject(this, &UAbilityTask_CreateTargetData::OnTargetDataReplicatedCallback);

		// If above delegate has been broadcast before binding, then call it again. Otherwise, wait for the data to come through. 
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, PredictionKey);
		if (!bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

APlayerController* UAbilityTask_CreateTargetData::GetPlayerController() const
{
	check(Ability);
	return Ability->GetCurrentActorInfo()->PlayerController.Get();
}

void UAbilityTask_CreateTargetData::SendTargetDataLocally()
{
	// Create a new Scoped Prediction Window, which is a window for everything we do here locally is Predicted.
	// "Scoped" meaning that it's limited to the scope of where we are when we create the window, so basically this function.
	// This will generate new ScopedPredictionKey for ASC, which is used below.
	FScopedPredictionWindow ScopedPredictionWindow(AbilitySystemComponent.Get());

	// Create Target Data Handle and add Target Data to it.
	FGameplayAbilityTargetDataHandle DataHandle;
	SetupTargetData(DataHandle);

	// Replicate Target Data to Server
	AbilitySystemComponent->ServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(),
		DataHandle, FGameplayTag(), AbilitySystemComponent->ScopedPredictionKey);

	// Still broadcast Data to Client, since this is Predicted.
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnReceivedTargetData.Broadcast(DataHandle);
	}
}

void UAbilityTask_CreateTargetData::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag) const
{
	// Let ASC know the Data is received now, so clear the cached data
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	// Broadcast to Server
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnReceivedTargetData.Broadcast(DataHandle);
	}
}
