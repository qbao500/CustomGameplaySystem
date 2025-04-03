// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_CreateTargetData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReceivedTargetData, const FGameplayAbilityTargetDataHandle&, DataHandle);

/**
 * A more simple version of WaitTargetData, which needs to spawn specific Actors and have the ability to Confirm/Cancel.
 * You need to subclass this to create your own, and override SetupTargetData(FGameplayAbilityTargetDataHandle& DataHandle)
 * Please check UAbilityTask_TargetDataUnderMouse as example of implementation
 */
UCLASS(Abstract)
class CUSTOMGAMEPLAYSYSTEM_API UAbilityTask_CreateTargetData : public UAbilityTask
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable)
	FReceivedTargetData OnReceivedTargetData;

protected:
	
	//~ Begin UGameplayTask interface
	virtual void Activate() override;
	//~ End UGameplayTask interface

	virtual void SetupTargetData(FGameplayAbilityTargetDataHandle& DataHandle) {}
	
	// Helpers
	APlayerController* GetPlayerController() const;

private:

	void SendTargetDataLocally();
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag) const;
};
