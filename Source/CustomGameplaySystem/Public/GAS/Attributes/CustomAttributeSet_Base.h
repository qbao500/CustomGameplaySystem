// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "CustomAttributeSet_Base.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName) 

USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()
	
	FGameplayEffectContextHandle EffectContextHandle;

	// Source = causer of this effect
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC = nullptr;
	UPROPERTY()
	TObjectPtr<AActor> SourceAvatarActor = nullptr;
	UPROPERTY()
	TObjectPtr<AController> SourceController = nullptr;
	UPROPERTY()
	TObjectPtr<ACharacter> SourceCharacter = nullptr;

	// Target = receiver of the effect (owner of this Attribute Set)
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetASC = nullptr;
	UPROPERTY()
	TObjectPtr<AActor> TargetAvatarActor = nullptr;
	UPROPERTY()
	TObjectPtr<AController> TargetController = nullptr;
	UPROPERTY()
	TObjectPtr<ACharacter> TargetCharacter = nullptr;
};

/** 
 * Delegate used to broadcast attribute events, some of these parameters may be null on clients: 
 * @param EffectInstigator	The original instigating actor for this event
 * @param EffectCauser		The physical actor that caused the change
 * @param EffectSpec		The full effect spec for this change
 * @param EffectMagnitude	The raw magnitude, this is before clamping
 * @param OldValue			The value of the attribute before it was changed
 * @param NewValue			The value after it was changed
*/
DECLARE_MULTICAST_DELEGATE_SixParams(FCustomAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/, float /*OldValue*/, float /*NewValue*/);

/**
 * Base abstract class for all attribute sets in the game.
 * For primary attributes, please add corresponding Gameplay Tags.
 * Then subclass UCustomAbilitySystemGlobals (change in DefaultGame.ini to use that class).
 * Within new class, override InitAttributeTags() to add new attribute tags.
 * These tags can be used for other features, such as upgrading attributes, saving/loading, etc.
 * TODO: Find a way to simplify more of this process.
 */
UCLASS(Abstract)
class CUSTOMGAMEPLAYSYSTEM_API UCustomAttributeSet_Base : public UAttributeSet
{
	GENERATED_BODY()

public:

	//~ Begin UObject interface
	virtual UWorld* GetWorld() const override;
	//~ End UObject interface

	//~ Begin UAttributeSet interface
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//~ End UAttributeSet interface

	virtual void ClampAttributeOnChange(const FGameplayAttribute& Attribute, float& NewValue) const {}
	
};

