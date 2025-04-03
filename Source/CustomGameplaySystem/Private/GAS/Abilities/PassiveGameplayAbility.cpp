// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/PassiveGameplayAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PassiveGameplayAbility)

UPassiveGameplayAbility::UPassiveGameplayAbility()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	
	ActivationPolicy = EAbilityActivationPolicy::OnSpawn;
}
