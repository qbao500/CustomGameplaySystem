// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CustomHealthComponent.generated.h"

struct FGameplayEffectSpec;
class UHealthAttributeSet;
class UCustomAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCustomHealth_Death, AActor*, OwningActor, const AActor*, DeathInstigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FCustomHealth_AttributeChanged, UCustomHealthComponent*, HealthComponent,
	float, OldValue, float, NewValue, AActor*, Instigator, AActor*, Causer);

UENUM(BlueprintType)
enum class EDeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class CUSTOMGAMEPLAYSYSTEM_API UCustomHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UCustomHealthComponent();
	
	// Returns the health component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "Custom|Health")
	static UCustomHealthComponent* FindHealthComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UCustomHealthComponent>() : nullptr); }

	// Initialize the component using an ability system component.
	UFUNCTION(BlueprintCallable, Category = "Custom|Health")
	void InitializeWithAbilitySystem(UCustomAbilitySystemComponent* InASC);

	// Uninitialize the component, clearing any references to the ability system.
	UFUNCTION(BlueprintCallable, Category = "Custom|Health")
	void UninitializeFromAbilitySystem();
	
	// Returns the current health value.
	UFUNCTION(BlueprintCallable, Category = "Custom|Health")
	float GetHealth() const;

	// Returns the current maximum health value.
	UFUNCTION(BlueprintCallable, Category = "Custom|Health")
	float GetMaxHealth() const;

	// Returns the current health in the range [0.0, 1.0].
	UFUNCTION(BlueprintCallable, Category = "Custom|Health")
	float GetHealthNormalized() const;

	UFUNCTION(BlueprintCallable, Category = "Custom|Health")
	EDeathState GetDeathState() const { return DeathState; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Custom|Health", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadOrDying() const { return (DeathState > EDeathState::NotDead); }

	// Begins the death sequence for the owner.
	virtual void StartDeath(const AActor* DeathInstigator);

	// Ends the death sequence for the owner.
	virtual void FinishDeath();

	// Delegate fired when the health value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FCustomHealth_AttributeChanged OnHealthChanged;

	// Delegate fired when the max health value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FCustomHealth_AttributeChanged OnMaxHealthChanged;

	// Delegate fired when the death sequence has started.
	// DeathInstigator can be null on Client.
	UPROPERTY(BlueprintAssignable)
	FCustomHealth_Death OnDeathStarted;

	// Delegate fired when the death sequence has finished.
	// Death Instigator always null, for now.
	UPROPERTY(BlueprintAssignable)
	FCustomHealth_Death OnDeathFinished;

protected:

	virtual void BeginPlay() override;
	virtual void OnUnregister() override;
	
	virtual void HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);

	UFUNCTION()
	virtual void OnRep_DeathState(EDeathState OldDeathState);
	
private:

	UPROPERTY()
	TObjectPtr<UCustomAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<const UHealthAttributeSet> HealthSet;

	// Replicated state used to handle dying.
	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	EDeathState DeathState = EDeathState::NotDead;

	void ClearDeadTags() const;
};
