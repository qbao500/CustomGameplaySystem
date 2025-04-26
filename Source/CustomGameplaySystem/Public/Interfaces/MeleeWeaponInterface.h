// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"
#include "MeleeWeaponInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMeleeWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CUSTOMGAMEPLAYSYSTEM_API IMeleeWeaponInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Melee Weapon")
	void OnLightAttack();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Melee Weapon")
	void OnHeavyAttack();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Melee Weapon")
	void OnSheath();
};
