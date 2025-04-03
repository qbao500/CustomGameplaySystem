// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProjectileShooterInterface.generated.h"

struct FGameplayTagContainer;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UProjectileShooterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CUSTOMGAMEPLAYSYSTEM_API IProjectileShooterInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Projectile Shooter")
	TArray<UPrimitiveComponent*> GetShooterColliders();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Projectile Shooter")
	FVector GetProjectileSpawnLocation() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Projectile Shooter", meta = (AutoCreateRefTerm = "AbilityTags, ActivationOwnedTags"))
	FVector GetProjectileSpawnLocationForAbility(const FGameplayTagContainer& AbilityTags, const FGameplayTagContainer& ActivationOwnedTags) const;
};
