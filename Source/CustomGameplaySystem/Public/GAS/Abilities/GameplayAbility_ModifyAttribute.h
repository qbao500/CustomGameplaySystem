// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomGameplayTags.h"
#include "GAS/Abilities/CustomGameplayAbility.h"
#include "GameplayAbility_ModifyAttribute.generated.h"

/**
 * This Ability is a passive one, that listens to Attribute (tag) Events and modifies the corresponding Attribute.
 * Make a BP subclass of this, and specify which Attribute to modify with AttributeGameplayEffect.
 * In AttributeGameplayEffect, those Attributes need to be "Set By Caller".
 * When you want to modify an Attribute, call UAbilitySystemBlueprintLibrary::SendGameplayEventToActor,
 * with EventTag as the Attribute tag, and EventMagnitude in Payload as the value to modify.
 * NOTE: DEPRECATED! Will be removed in the future.
 */
UCLASS(Abstract, Deprecated, NotBlueprintable)
class CUSTOMGAMEPLAYSYSTEM_API UDEPRECATED_GameplayAbility_ModifyAttribute : public UCustomGameplayAbility
{
	GENERATED_BODY()

public:

	UDEPRECATED_GameplayAbility_ModifyAttribute();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	// The Gameplay Effect to apply when the Attribute Event is received.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> AttributeGameplayEffect;

	// Should be the parent tag of Attribute(s).
	// Default value is already good to use.
	// Then any child tag of this tag will trigger the event.
	UPROPERTY(VisibleDefaultsOnly)
	FGameplayTag AttributeTagToListen = CustomTags::Attribute;

	UFUNCTION()
	virtual void OnAttributeEventReceived(FGameplayEventData Payload);

private:

	// Retrieve these tags from AttributeGameplayEffect.
	TArray<FGameplayTag> SetByCallerTags;
};
