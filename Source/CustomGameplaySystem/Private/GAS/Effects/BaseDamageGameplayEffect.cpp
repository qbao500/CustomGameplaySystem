// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Effects/BaseDamageGameplayEffect.h"

#include "GAS/ExecutionCalculation/ExecCalc_SimpleDamage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BaseDamageGameplayEffect)

UBaseDamageGameplayEffect::UBaseDamageGameplayEffect()
{
	FGameplayEffectExecutionDefinition SimpleDamageDef;
	SimpleDamageDef.CalculationClass = UExecCalc_SimpleDamage::StaticClass();
	Executions.Emplace(SimpleDamageDef);
}