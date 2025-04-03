// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/DamageGameplayAbility.h"
#include "MeleeGameplayAbility.generated.h"

/**
 * A DamageGameplayAbility that is specialized for Melee.
 * This can wait for Gameplay Event to deal damage based on Hit Result.
 * HitboxComponent is designed to work with this, but not a must.
 */
UCLASS(Abstract)
class CUSTOMGAMEPLAYSYSTEM_API UMeleeGameplayAbility : public UDamageGameplayAbility
{
	GENERATED_BODY()

public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bAutoApplyDamageWhenReceiveEvent = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Event", EditCondition = "bAutoApplyDamageWhenReceiveEvent"))
	FGameplayTag DamageEventToListen = FGameplayTag::EmptyTag;

private:

	UFUNCTION()
	void OnReceiveHitboxEvent(FGameplayEventData Payload);
};
