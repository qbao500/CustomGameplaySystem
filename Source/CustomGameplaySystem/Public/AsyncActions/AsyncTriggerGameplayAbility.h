// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncTriggerGameplayAbility.generated.h"

struct FGameplayTagContainer;
class UGameplayAbility;
class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAsyncGameplayAbilityEnded);

/**
 *	This would be the main functionality for Behavior Tree Task that executes Gameplay Ability
 *	https://forums.unrealengine.com/t/gas-does-anyone-know-how-to-wait-for-the-ability-to-finish/478336/3
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class CUSTOMGAMEPLAYSYSTEM_API UAsyncTriggerGameplayAbility : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable)
	FAsyncGameplayAbilityEnded OnFinished;
	UPROPERTY(BlueprintAssignable)
	FAsyncGameplayAbilityEnded OnFailed;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTriggerGameplayAbility* AsyncTriggerAbilityByClass(UAbilitySystemComponent* AbilitySystemComponent, TSubclassOf<UGameplayAbility> AbilityClass);

	// Only the first one that is found
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTriggerGameplayAbility* AsyncTriggerAbilityByTag(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTagContainer& Tags);
	
	// You must call this function manually when you want the AsyncTask to end.
	// For UMG Widgets, you would call it in the Widget's Destruct event.
	UFUNCTION(BlueprintCallable)
	void EndTask();

protected:
	
	UPROPERTY()
	UGameplayAbility* AbilityListeningTo;
	FDelegateHandle DelegateHandle;

	bool bWillFail = false;
	
	UFUNCTION()
	virtual void OnAbilityEnded(UGameplayAbility* Ability);
	void OnAbilityFailed();

	virtual void Activate() override;
};
