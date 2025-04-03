// Fill out your copyright notice in the Description page of Project Settings.
// TODO Look into later: https://ukustra.medium.com/how-to-easily-bind-gameplay-ability-system-to-enhanced-input-b45ece2ca02b

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "CustomGameplayAbility.generated.h"

UENUM(BlueprintType)
enum class EAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn
};

/**
 * 
 */
UCLASS(Abstract, HideCategories = "Input")
class CUSTOMGAMEPLAYSYSTEM_API UCustomGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

	friend class UCustomAbilitySystemComponent;

public:

	UCustomGameplayAbility();

	//~ Begin UGameplayAbility interface
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	//~ End UGameplayAbility interface

	EAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	
	/**
	 * Return the cost based on CostGameplayEffectClass, if assigned
	 * @param Level Ability level to check
	 * @param AttributeToCheck Check matching attribute. If not defined, check all
	 */
	UFUNCTION(BlueprintPure, meta = (AutoCreateRefTerm = "AttributeToCheck"))
	virtual float GetAbilityCost(const int32 Level, const FGameplayAttribute& AttributeToCheck = FGameplayAttribute()) const;
	
	UFUNCTION(BlueprintPure)
	virtual float GetAbilityCooldown(const int32 Level) const;

	UFUNCTION(BlueprintPure)
	bool DoesEffectContextHaveThisAbility(const FGameplayEffectContextHandle& EffectContext) const;
	
	UFUNCTION(BlueprintNativeEvent)
	FText FillAbilityDescriptionWithFormat(const FText& Description, int32 Level);
	
protected:

	//~ Begin UGameplayAbility interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	//~ End UGameplayAbility interface

	// Defines how this ability is meant to activate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom|Ability Activation")
	EAbilityActivationPolicy ActivationPolicy;

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;
	
	virtual void OnPawnAvatarSet(APawn* NewAvatar);

	/**
	 * Support for BP, when this ability is added to ASC. Basically similar as "BeginPlay".
	 */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Give Ability")
	void K2_OnGiveAbility(const FGameplayAbilitySpec& Spec);
	
	/**
	 * Only trigger if this ability is connected with certain input (using input ID in FGameplayAbilitySpec)
	 * Also, only trigger if the ability is already active
	 */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Input Pressed")
	void K2_InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActivationInfo& ActivationInfo);

	/**
	 * Only trigger if this ability is connected with certain input (using input ID in FGameplayAbilitySpec)
	 */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Input Released")
	void K2_InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActivationInfo& ActivationInfo);

	/**
	 * Called when the ability system is initialized with a pawn avatar
	 */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Pawn Avatar Set")
	void K2_OnPawnAvatarSet(APawn* NewAvatar);

	UFUNCTION(BlueprintPure)
	FVector GetAvatarLocation() const;
	UFUNCTION(BlueprintPure)
	FVector GetAvatarForwardVector() const;
	UFUNCTION(BlueprintPure)
	FVector GetAvatarRightVector() const;
	UFUNCTION(BlueprintPure)
	FVector GetAvatarUpVector() const;
	UFUNCTION(BlueprintPure)
	FRotator GetAvatarRotation() const;
	UFUNCTION(BlueprintPure)
	FQuat GetAvatarQuat() const;
	UFUNCTION(BlueprintPure)
	FVector GetAvatarScale() const;
	UFUNCTION(BlueprintPure)
	const FTransform& GetAvatarTransform() const;

	UFUNCTION(BlueprintPure)
	USceneComponent* GetAvatarRootComponent() const;

	UFUNCTION(BlueprintPure)
	static const FGameplayTagContainer& GetDamageTags();

private:

	// Used for default implementation of FillAbilityDescriptionWithFormat to format the Cost value.
	UPROPERTY(EditDefaultsOnly, Category = "Description")
	FString CostKeyToFormat = "Cost";
	UPROPERTY(EditDefaultsOnly, Category = "Description")
	FString CooldownKeyToFormat = "Cooldown";
};
