// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CustomGameplayTags.h"
#include "GAS/Abilities/CustomGameplayAbility.h"
#include "GAS/Effects/BaseDamageGameplayEffect.h"
#include "DamageGameplayAbility.generated.h"

class UAbilityAsync_WaitGameplayEvent;

USTRUCT(BlueprintType)
struct FAbilityDamageInfo
{
	GENERATED_BODY()

	// Option to set Context. If not, then a new default Context will be made
	FGameplayEffectContextHandle DamageContextHandle = FGameplayEffectContextHandle();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FScalableFloat Damage = FScalableFloat(1.0f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UBaseDamageGameplayEffect> DamageGE = UBaseDamageGameplayEffect::StaticClass();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (Categories = "Damage"))
	FGameplayTag DamageTag = CustomTags::Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bRadialDamage = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "bRadialDamage", EditConditionHides))
	float RadialDamageFalloff = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "bRadialDamage", EditConditionHides))
	float RadialMinimumDamage = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "bRadialDamage", EditConditionHides))
	float RadialInnerRadius = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "bRadialDamage", EditConditionHides))
	float RadialOuterRadius = 200.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "bRadialDamage", EditConditionHides))
	float PhysicsRadialImpulseStrength = 500.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "bRadialDamage", EditConditionHides))
	bool bDoTracePreventRadialDamage = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "bRadialDamage && bDoTracePreventRadialDamage", EditConditionHides))
	TEnumAsByte<ECollisionChannel> RadialDamagePreventionChannel = ECC_Visibility;

	float GetCurrentDamage() const;
	float GetDamageAtLevel(const int32 Level) const;
	int32 GetAbilityLevel() const;

	UAbilitySystemComponent* GetAbilitySystemComponent() const;
	const UGameplayAbility* GetAbility() const;
};

/**
 * Base GA class (abstract) for Abilities that deal damage
 */
UCLASS(Abstract)
class CUSTOMGAMEPLAYSYSTEM_API UDamageGameplayAbility : public UCustomGameplayAbility
{
	GENERATED_BODY()

public:

	//~ Begin UCustomGameplayAbility Interface
	virtual FText FillAbilityDescriptionWithFormat_Implementation(const FText& Description, int32 Level) override;
	//~ End UCustomGameplayAbility Interface

	//~ Begin UGameplayAbility Interface
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	//~ End UGameplayAbility Interface

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", meta = (ShowOnlyInnerProperties))
	FAbilityDamageInfo DamageInfo;

	// If true, this ability will listen to Event.Hit when it hits target with ASC
	// Override OnHitTargetEvent to implement custom behavior
	// Note: This uses Async Action, not Ability Task, so it can keep listening after the ability ends
	UPROPERTY(EditDefaultsOnly, Category = "Hit Event")
	bool bAutoListenToHitEvent = false;
	UPROPERTY(EditDefaultsOnly, Category = "Hit Event", meta = (EditCondition = "bAutoListenToHitEvent", EditConditionHides))
	TArray<TSubclassOf<UGameplayEffect>> EffectsToAutoApplyToHitTarget;

	// Event when this ability hits a target with ASC, if bAutoListenToHitEvent is true
	// Retrieve damage dealt from Payload.Magnitude
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnHitTargetEvent", Category = "Hit Event")
	void K2_OnHitTargetEvent(const FGameplayEventData& Payload);
	UFUNCTION()
	virtual void OnHitTargetEvent(FGameplayEventData Payload);

	UFUNCTION(BlueprintPure)
	float GetTotalDamage() const;
	UFUNCTION(BlueprintPure)
	float GetTotalDamageAtLevel(const int32 Level) const;
	
	UFUNCTION(BlueprintCallable)
	void AssignDamageToEffectSpec(const FGameplayEffectSpecHandle& SpecHandle) const;
	UFUNCTION(BlueprintCallable)
	void AssignCustomDamageToEffectSpec(const FGameplayEffectSpecHandle& SpecHandle, const float CustomDamage);

	UFUNCTION(BlueprintCallable)
	void ApplyDamage(AActor* TargetActor, const FHitResult& HitResult);
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "IgnoreActors"))
	bool ApplyRadialDamage(const FVector& Origin, const TArray<AActor*>& IgnoreActors);

private:

	void AutoApplyEffectsOnHit(const FGameplayEventData& Payload);

	// Used for default implementation of FillAbilityDescriptionWithFormat to format the Damage value.
	UPROPERTY(EditDefaultsOnly, Category = "Description")
	FString DamageKeyToFormat = "Damage";

	UPROPERTY()
	UAbilityAsync_WaitGameplayEvent* WaitHitEventAction = nullptr;
};