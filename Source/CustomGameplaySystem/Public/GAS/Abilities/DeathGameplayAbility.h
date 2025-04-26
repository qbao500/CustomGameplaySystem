// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GAS/Abilities/CustomGameplayAbility.h"
#include "DeathGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UDeathGameplayAbility : public UCustomGameplayAbility
{
	GENERATED_BODY()

public:

	UDeathGameplayAbility();

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	// Starts the death sequence.
	UFUNCTION(BlueprintCallable)
	void StartDeath(const AActor* DeathInstigator);

	// Finishes the death sequence.
	UFUNCTION(BlueprintCallable)
	void FinishDeath();
	
private:
	
	// If enabled, the ability will automatically call StartDeath.  FinishDeath is always called when the ability ends if the death was started.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bAutoStartDeath = true;
};
