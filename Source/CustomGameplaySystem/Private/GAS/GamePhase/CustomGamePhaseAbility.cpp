// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GamePhase/CustomGamePhaseAbility.h"

#include "AbilitySystemComponent.h"
#include "GAS/GamePhase/CustomGamePhaseSubsystem.h"
#include "Misc/DataValidation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomGamePhaseAbility)

UCustomGamePhaseAbility::UCustomGamePhaseAbility()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnly;

	ActivationPolicy = EAbilityActivationPolicy::OnSpawn;
}

const FGameplayTag& UCustomGamePhaseAbility::GetGamePhaseTag() const
{
	return GamePhaseTag;
}

void UCustomGamePhaseAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	if (ActorInfo->IsNetAuthority())
	{
		const UWorld* World = ActorInfo->AbilitySystemComponent->GetWorld();
		UCustomGamePhaseSubsystem* PhaseSubsystem = UWorld::GetSubsystem<UCustomGamePhaseSubsystem>(World);
		PhaseSubsystem->OnBeginPhase(this, Handle);
	}
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UCustomGamePhaseAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo->IsNetAuthority())
	{
		const UWorld* World = ActorInfo->AbilitySystemComponent->GetWorld();
		UCustomGamePhaseSubsystem* PhaseSubsystem = UWorld::GetSubsystem<UCustomGamePhaseSubsystem>(World);
		PhaseSubsystem->OnEndPhase(this, Handle);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

#if WITH_EDITOR
EDataValidationResult UCustomGamePhaseAbility::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	if (!GamePhaseTag.IsValid())
	{
		Result = EDataValidationResult::Invalid;
		Context.AddError(FText::FromString("GamePhaseTag must be set to a tag representing the current phase!"));
	}

	return Result;
}
#endif
