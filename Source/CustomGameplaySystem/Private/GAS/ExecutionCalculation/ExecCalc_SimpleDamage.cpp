// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/ExecutionCalculation/ExecCalc_SimpleDamage.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CustomGameplayTags.h"
#include "GAS/CustomAbilitySystemLibrary.h"
#include "FunctionLibraries/CustomHelperFunctionLibrary.h"
#include "GAS/Attributes/HealthAttributeSet.h"

struct FDamageStatics
{
	// FGameplayEffectAttributeCaptureDefinition CritChanceDef;
	
	FDamageStatics()
	{
		// CritChanceDef = FGameplayEffectAttributeCaptureDefinition(UAuraAttributeSet::GetCriticalHitChanceAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
	}
};

static const FDamageStatics& DamageStatics()
{
	// Initialize only once with "static"
	// Everytime this function is called, it will return the same instance of Statics
	static FDamageStatics Statics;
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
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	if (!SourceASC || !TargetASC)
	{
		return;
	}

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FAggregatorEvaluateParameters EvaluateParameters = UCustomAbilitySystemLibrary::GetAggregatorEvaluateParamsByEffectSpec(Spec);

	// Get Damage Set by Caller Magnitude
	const float Damage = Spec.GetSetByCallerMagnitude(CustomTags::Damage);

	// Example of Crit Chance
	/*
	float SourceCritChance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritChanceDef, EvaluateParameters, SourceCritChance);
	SourceCritChance = FMath::Max(0.0f, SourceCritChance);
	if (GFL::RandomWithChance(SourceCritChance))
	{
		Damage *= 2.0f;
	}
	*/

	// Apply the Damage to the Target via meta attribute of IncomingDamageAttribute
	const FGameplayModifierEvaluatedData EvaluatedData(UHealthAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
	
	// Notify SourceASC about the Hit
	FGameplayEventData Payload;
	Payload.EventTag = CustomTags::Event_Hit;
	Payload.EventMagnitude = Damage;
	Payload.Instigator = SourceASC->GetOwnerActor();
	Payload.Target = TargetASC->GetOwnerActor();
	Payload.InstigatorTags = *EvaluateParameters.SourceTags;
	Payload.TargetTags = *EvaluateParameters.TargetTags;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SourceASC->GetOwnerActor(), CustomTags::Event_Hit, Payload);
}
