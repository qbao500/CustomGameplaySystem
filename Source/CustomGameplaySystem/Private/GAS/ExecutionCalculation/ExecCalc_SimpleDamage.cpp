// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/ExecutionCalculation/ExecCalc_SimpleDamage.h"

#include "AbilitySystemComponent.h"
#include "CustomGameplayTags.h"
#include "GAS/CustomAbilitySystemLibrary.h"
#include "GAS/Attributes/HealthAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ExecCalc_SimpleDamage)

struct FSimpleDamageStatics
{
	// FGameplayEffectAttributeCaptureDefinition CritChanceDef;
	
	FSimpleDamageStatics()
	{
		// CritChanceDef = FGameplayEffectAttributeCaptureDefinition(UAuraAttributeSet::GetCriticalHitChanceAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
	}
};

static const FSimpleDamageStatics& SimpleDamageStatics()
{
	// Initialize only once with "static"
	// Everytime this function is called, it will return the same instance of Statics
	static FSimpleDamageStatics Statics;
	return Statics;
}

UExecCalc_SimpleDamage::UExecCalc_SimpleDamage()
{
	// Example of Crit Chance
	/*
	RelevantAttributesToCapture.Emplace(DamageStatics().CritChanceDef);
	*/
}

// No need to call Super
void UExecCalc_SimpleDamage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	if (!SourceASC || !TargetASC)
	{
		return;
	}

	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();
	const FAggregatorEvaluateParameters EvaluateParameters = AFL::GetAggregatorEvaluateParamsByEffectSpec(EffectSpec);

	// Get Damage Set by Caller Magnitude
	const float Damage = EffectSpec.GetSetByCallerMagnitude(CustomTags::Damage);

	// Example of Crit Chance
	/*
	float SourceCritChance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritChanceDef, EvaluateParameters, SourceCritChance);
	SourceCritChance = FMath::Max(0.0f, SourceCritChance);
	if (HFL::RandomWithChance(SourceCritChance))
	{
		Damage *= 2.0f;
	}
	*/

	// Apply the Damage to the Target via meta attribute of IncomingDamageAttribute
	const FGameplayModifierEvaluatedData EvaluatedData(UHealthAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
