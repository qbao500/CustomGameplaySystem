// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "CustomAttributeSet_Base.h"
#include "HealthAttributeSet.generated.h"

/**
 *  Already added in CustomPlayerState
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UHealthAttributeSet : public UCustomAttributeSet_Base
{
	GENERATED_BODY()

public:
	
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, HealthRegeneration);
	
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, IncomingDamage);
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, IncomingHealing);

	// Delegate when health changes due to damage/healing, some information may be missing on the client
	mutable FCustomAttributeEvent OnHealthChanged;

	// Delegate when max health changes
	mutable FCustomAttributeEvent OnMaxHealthChanged;

	// Delegate to broadcast when the health attribute reaches zero
	mutable FCustomAttributeEvent OnOutOfHealth;

protected:

	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	virtual void ClampAttributeOnChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const;
	UFUNCTION()
	void OnRep_HealthRegeneration(const FGameplayAttributeData& OldValue) const;

private:

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Attributes|Health", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Attributes|Health", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth = 100.0f;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegeneration, Category = "Attributes|Health", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HealthRegeneration;

	// Used to track when the health reaches 0.
	bool bOutOfHealth = false;

	// Store the health before any changes 
	float HealthBeforeAttributeChange = 0.0f;
	float MaxHealthBeforeAttributeChange = 0.0f;

	// Meta Attribute, used as an intermediary, to perform necessary math, before actually setting value of real Attribute.
	// In this case, IncomingDamage is calculated before setting Health.
	// No replication needed, since it's just a temporary placeholder for calculation.
	// As Meta Attribute, it should be used for its value and then get zeroed out (check PostGameplayEffectExecute).
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData IncomingDamage;
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData IncomingHealing;
};
