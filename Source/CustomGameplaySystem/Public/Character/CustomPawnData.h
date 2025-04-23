// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CustomPawnData.generated.h"

class UCustomAbilityTagRelationship;
class UCustomInputConfig;
class UCustomAbilitySet;

/**
 * UCustomPawnData
 *
 * Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS(BlueprintType, Const, meta = (DisplayName = "Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class CUSTOMGAMEPLAYSYSTEM_API UCustomPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom|Abilities")
	TArray<TObjectPtr<UCustomAbilitySet>> AbilitySets;
	
	// What mapping of ability tags to use for actions taking by this pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom|Abilities")
	TObjectPtr<UCustomAbilityTagRelationship> TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom|Input")
	TObjectPtr<UCustomInputConfig> InputConfig;
};
