// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "Subsystems/WorldSubsystem.h"
#include "CustomGlobalAbilitySubsystem.generated.h"

class UGameplayEffect;
class UGameplayAbility;
class UCustomAbilitySystemComponent;

USTRUCT()
struct FGlobalAppliedAbilityList
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<TObjectPtr<UCustomAbilitySystemComponent>, FGameplayAbilitySpecHandle> Handles;

	void AddToASC(const TSubclassOf<UGameplayAbility>& Ability, UCustomAbilitySystemComponent* ASC);
	void RemoveFromASC(UCustomAbilitySystemComponent* ASC);
	void RemoveFromAll();
};

USTRUCT()
struct FGlobalAppliedEffectList
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<TObjectPtr<UCustomAbilitySystemComponent>, FActiveGameplayEffectHandle> Handles;

	void AddToASC(const TSubclassOf<UGameplayEffect>& Effect, UCustomAbilitySystemComponent* ASC);
	void RemoveFromASC(UCustomAbilitySystemComponent* ASC);
	void RemoveFromAll();
};

/**
 * A subsystem to apply/remove Abilities and Effects to all Custom Ability System Components globally.
 * Note: This is a UWorldSubsystem, NOT a subclass of UAbilitySystemGlobals (which is a UObject).
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UCustomGlobalAbilitySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect);

	/** Register an ASC with global system and apply any active global effects/abilities. */
	void RegisterASC(UCustomAbilitySystemComponent* ASC);

	/** Removes an ASC from the global system, along with any active global effects/abilities. */
	void UnregisterASC(UCustomAbilitySystemComponent* ASC);

private:
	
	UPROPERTY()
	TMap<TSubclassOf<UGameplayAbility>, FGlobalAppliedAbilityList> AppliedAbilities;

	UPROPERTY()
	TMap<TSubclassOf<UGameplayEffect>, FGlobalAppliedEffectList> AppliedEffects;

	UPROPERTY()
	TSet<TObjectPtr<UCustomAbilitySystemComponent>> RegisteredASCs;
};
