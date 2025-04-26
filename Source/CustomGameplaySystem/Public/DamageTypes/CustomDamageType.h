// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "GameFramework/DamageType.h"
#include "CustomDamageType.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UCustomDamageType : public UDamageType
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer DamageTags = FGameplayTagContainer();

	// If > 0, then use this (for now only work on Player)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KnockBackForce = -1.0f;

	UFUNCTION(BlueprintCallable)
	void ModifyKnockBackForce(UPARAM(ref) float& ForceRef);

	UFUNCTION(BlueprintPure)
	bool HasTag(const FGameplayTag Tag) const;
	bool HasTag(const FName& Tag) const;
};
