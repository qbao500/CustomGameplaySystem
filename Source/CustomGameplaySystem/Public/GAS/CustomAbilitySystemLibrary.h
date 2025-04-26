// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CustomAbilitySystemLibrary.generated.h"

#define AFL UCustomAbilitySystemLibrary

struct FAbilityDamageInfo;
class UGameplayAbility;
class UGameplayEffect;
class UAbilitySystemComponent;
struct FGameplayAbilitySpec;
struct FGameplayEffectContextHandle;
struct FGameplayEffectSpec;
struct FAggregatorEvaluateParameters;

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UCustomAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "DataTag"))
	static void ApplyEffectSetByCaller(UAbilitySystemComponent* ASC, const TSubclassOf<UGameplayEffect>& Effect, const FGameplayTag& DataTag, const float Magnitude, UObject* OptionalSourceObject = nullptr);
	static void ApplyEffectSetByCaller(UAbilitySystemComponent* ASC, const TSubclassOf<UGameplayEffect>& Effect, const FName& DataTagName, const float Magnitude, UObject* OptionalSourceObject = nullptr);

	UFUNCTION(BlueprintPure)
	static TArray<FGameplayTag> GetSetByCallerTags(const TSubclassOf<UGameplayEffect>& FromGameplayEffect);

	UFUNCTION(BlueprintPure)
	static FGameplayEffectContextHandle MakeEffectContextWithObject(const UAbilitySystemComponent* ASC, const UObject* SourceObject);

	UFUNCTION(BlueprintPure, meta = (Categories = "Ability", AutoCreateRefTerm = "AbilityTag"))
	static FGameplayAbilitySpec GetAbilitySpecFromTag(UAbilitySystemComponent* ASC, const FGameplayTag& AbilityTag, bool& bFound);
	static FGameplayAbilitySpec* GetAbilitySpecFromTag(UAbilitySystemComponent* ASC, const FGameplayTag& AbilityTag);
	
	UFUNCTION(BlueprintPure)
	static UGameplayAbility* GetAbilityInstanceFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	UFUNCTION(BlueprintPure)
	static const FGameplayTagContainer& GetAbilityTagsFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	UFUNCTION(BlueprintPure)
	static const FGameplayTag& GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	UFUNCTION(BlueprintPure, meta = (Categories = "Ability", AutoCreateRefTerm = "AbilityTag"))
	static bool IsAbilityTagInSpec(const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& AbilityTag);
	UFUNCTION(BlueprintPure)
	static const FGameplayTag& GetFirstAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	UFUNCTION(BlueprintPure)
	static const FGameplayTag& GetFirstAbilityTagFromAbility(const TSubclassOf<UGameplayAbility>& Ability);
	UFUNCTION(BlueprintPure)
	static const FGameplayTagContainer& GetDynamicAbilityTagsFromSpec(const FGameplayAbilitySpec& AbilitySpec);

	UFUNCTION(BlueprintPure, meta = (Categories = "Input", AutoCreateRefTerm = "InputSlot"))
	static bool DoesAbilityHaveInputTag(const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& InputTag);
	static bool DoesAbilityHaveInputTag(const FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& InputTag);

	UFUNCTION(BlueprintCallable)
	static void AbilityApplyDamage(const UGameplayAbility* Ability, const FAbilityDamageInfo& AbilityDamageInfo,
		AActor* TargetActor, const FHitResult& HitResult);
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "IgnoredActors"))
	static bool AbilityApplyRadialDamage(const UGameplayAbility* Ability, const FAbilityDamageInfo& AbilityDamageInfo,
		const FVector& Origin, const TArray<AActor*>& IgnoredActors);

	UFUNCTION(BlueprintPure, meta = (Categories = "Ability", AutoCreateRefTerm = "AbilityTag"))
	static FPrimaryAssetId MakeAbilityAssetId(const FGameplayTag& AbilityTag);

	/** C++ only */

	static FGameplayAbilitySpec* GetAbilitySpecFromInput(UAbilitySystemComponent* ASC, const FGameplayTag& InputTag);
	
	static FAggregatorEvaluateParameters GetAggregatorEvaluateParamsByEffectSpec(const FGameplayEffectSpec& Spec);
};
