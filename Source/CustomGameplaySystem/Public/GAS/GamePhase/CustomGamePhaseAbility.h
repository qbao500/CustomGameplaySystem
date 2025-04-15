// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/CustomGameplayAbility.h"
#include "CustomGamePhaseAbility.generated.h"

/**
 * The base gameplay ability for any ability that is used to change the active game phase.
 */
UCLASS(Abstract, HideCategories = "Input, Tags, Costs, Cooldowns, Triggers")
class CUSTOMGAMEPLAYSYSTEM_API UCustomGamePhaseAbility : public UCustomGameplayAbility
{
	GENERATED_BODY()

public:

	UCustomGamePhaseAbility();

	const FGameplayTag& GetGamePhaseTag() const;

protected:

	//~ Begin UGameplayAbility interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~ End UGameplayAbility interface

	// Defines the game phase that this game phase ability is part of.  So for example,
	// if your game phase is GamePhase.RoundStart, then it will cancel all sibling phases.
	// So if you had a phase such as GamePhase.WaitingToStart that was active, starting
	// the ability part of RoundStart would end WaitingToStart.  However to get nested behaviors
	// you can also nest the phases.  So for example, GamePhase.Playing.NormalPlay, is a sub-phase
	// of the parent GamePhase.Playing, so changing the sub-phase to GamePhase.Playing.SuddenDeath,
	// would stop any ability tied to GamePhase.Playing.*, but wouldn't end any ability 
	// tied to the GamePhase.Playing phase.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom|GamePhase", meta = (Categories = "GamePhase"))
	FGameplayTag GamePhaseTag;

public:

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
