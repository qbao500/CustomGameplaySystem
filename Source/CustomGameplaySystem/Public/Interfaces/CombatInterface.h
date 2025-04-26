// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

class UCapsuleComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
	
};

/**
 * 
 */
class CUSTOMGAMEPLAYSYSTEM_API ICombatInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	void StartAttack(AActor* Target);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	void StartBlocking();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	void AttackGetBlocked(const AActor* Blocker);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	void ReceiveParry(AActor* ParryCauser, const float ParryMagnitude);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	AActor* GetCombatTarget() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	void SetCombatTarget(AActor* NewTarget);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	UCapsuleComponent* GetCapsule() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	USkeletalMeshComponent* GetMainMesh() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	void GetWeaponCompAndSocket(USceneComponent*& WeaponSceneComp, FName& WeaponSocket) const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	FVector GetCombatSocketLocation() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	FVector GetFootLocation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	bool IsAlive();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	bool IsImmuneToDamage() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	bool IsOnAir() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	float GetPushbackMultiplier() const;
};
