// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_SimpleDamage.generated.h"

/**
 * A simple sample of how this class is coded, to apply damage to Health Attribute.
 * Most likely you would need to create a more complex one for your game, which has more attributes like Critical Chance, Armor, etc.
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UExecCalc_SimpleDamage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UExecCalc_SimpleDamage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
