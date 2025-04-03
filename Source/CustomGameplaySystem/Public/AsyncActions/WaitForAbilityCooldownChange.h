// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/CancellableAsyncAction.h"
#include "WaitForAbilityCooldownChange.generated.h"

class UGameplayAbility;
class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownChange, const float, TimeRemaining);

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UWaitForAbilityCooldownChange : public UCancellableAsyncAction
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Ability|AsyncAction", meta = (BlueprintInternalUseOnly = "true", AutoCreateRefTerm = "InCooldownTag"))
	static UWaitForAbilityCooldownChange* WaitForAbilityCooldownChange(UAbilitySystemComponent* ASC, const FGameplayTag& InCooldownTag);

	UPROPERTY(BlueprintAssignable)
	FCooldownChange OnCooldownStart;
	UPROPERTY(BlueprintAssignable)
	FCooldownChange OnCooldownEnd;

	virtual void Cancel() override;
	
private:

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	FGameplayTag CooldownTag = FGameplayTag::EmptyTag;

	void CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount);
};
