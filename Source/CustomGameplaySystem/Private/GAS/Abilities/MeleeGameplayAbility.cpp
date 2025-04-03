// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/MeleeGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MeleeGameplayAbility)

void UMeleeGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (bAutoApplyDamageWhenReceiveEvent && DamageEventToListen.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, DamageEventToListen);
		Task->EventReceived.AddDynamic(this, &ThisClass::OnReceiveHitboxEvent);
		Task->ReadyForActivation();
	}
}

void UMeleeGameplayAbility::OnReceiveHitboxEvent(FGameplayEventData Payload)
{
	const FHitResult& HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(Payload.TargetData, 0);

	if (HitResult.GetActor())
	{
		ApplyDamage(HitResult.GetActor(), HitResult);
	}
}
