// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CustomCharacterBase.h"
#include "CustomCharacterWithAbilities.generated.h"

class UHealthAttributeSet;

/**
 * ACustomCharacterBase typically gets the Ability System Component from the possessing player state.
 * This represents a character with a self-contained Ability System Component
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API ACustomCharacterWithAbilities : public ACustomCharacterBase
{
	GENERATED_BODY()

public:

	ACustomCharacterWithAbilities();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	//~ Begin IAbilitySystemInterface interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface interface

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCustomAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UHealthAttributeSet> HealthSet;
};
