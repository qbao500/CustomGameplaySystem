// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyCombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UEnemyCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CUSTOMGAMEPLAYSYSTEM_API IEnemyCombatInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy Combat")
	bool IsEnemyAttacking() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy Combat")
	float GetCurrentPosture() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy Combat")
	void LosePosture(const float PostureToLose, const bool bShouldStagger);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy Combat")
	FName GetFinisherBone() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy Combat")
	FVector GetFinisherLocation() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy Combat")
	void BeginReceiverFinisher(AActor* FinishedBy);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy Combat")
	void ReceiveFinisher(AActor* FinishedBy);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy Combat")
	void OnNearToPlayer();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy Combat")
	void OnAwayFromPlayer();
};
