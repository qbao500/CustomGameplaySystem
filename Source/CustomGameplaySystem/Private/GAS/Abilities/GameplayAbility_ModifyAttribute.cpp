// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GameplayAbility_ModifyAttribute.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GAS/CustomAbilitySystemLibrary.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayAbility_ModifyAttribute)

UDEPRECATED_GameplayAbility_ModifyAttribute::UDEPRECATED_GameplayAbility_ModifyAttribute()
{
	// Modify Attribute should be only Server job.
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UDEPRECATED_GameplayAbility_ModifyAttribute::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!AttributeGameplayEffect) return;

	SetByCallerTags = UCustomAbilitySystemLibrary::GetSetByCallerTags(AttributeGameplayEffect);

	if (UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, AttributeTagToListen,
		nullptr, false, false))
	{
		Task->EventReceived.AddDynamic(this, &UDEPRECATED_GameplayAbility_ModifyAttribute::OnAttributeEventReceived);
		Task->ReadyForActivation();
	}
}

void UDEPRECATED_GameplayAbility_ModifyAttribute::OnAttributeEventReceived(FGameplayEventData Payload)
{
	PLFL::PrintWarning(Payload.EventTag.ToString());
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	check(ASC);
	
	const FGameplayEffectSpecHandle& SpecHandle = ASC->MakeOutgoingSpec(AttributeGameplayEffect, 1, ASC->MakeEffectContext());
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (!Spec) return;

	const FGameplayTag& AttributeEventTag = Payload.EventTag;
	
	for (const FGameplayTag& Tag : SetByCallerTags)
	{
		if (!Tag.IsValid()) continue;

		// We only want to set the magnitude of the tag that matches the event tag.
		// But still need to set magnitude to 0 for others.
		// Otherwise, it will throw error of not calling SetByCaller for other tags.
		const float Magnitude = Tag == AttributeEventTag ? Payload.EventMagnitude : 0.0f;
		Spec->SetSetByCallerMagnitude(Tag, Magnitude);
	}

	// Finally, apply the effect.
	ASC->ApplyGameplayEffectSpecToSelf(*Spec);
}
