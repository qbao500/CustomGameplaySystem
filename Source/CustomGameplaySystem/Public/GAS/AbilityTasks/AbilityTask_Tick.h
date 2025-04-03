// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_Tick.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTickTaskDelegate, float, DeltaTime);
 
/**
 * Task for abilities that supply tick and its delta time.
 */

UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UAbilityTask_Tick : public UAbilityTask
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FOnTickTaskDelegate OnTick;

public:
 
	UAbilityTask_Tick(const FObjectInitializer& ObjectInitializer);
 
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_Tick* AbilityTaskOnTick(UGameplayAbility* OwningAbility);
	
	virtual void TickTask(float DeltaTime) override;
	
};
