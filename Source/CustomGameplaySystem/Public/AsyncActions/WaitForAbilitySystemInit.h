// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CustomCorePawnComponent.h"
#include "Engine/CancellableAsyncAction.h"
#include "WaitForAbilitySystemInit.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UWaitForAbilitySystemInit : public UCancellableAsyncAction
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Ability|AsyncAction", meta = (BlueprintInternalUseOnly = "true"))
	static UWaitForAbilitySystemInit* WaitForAbilitySystemInit(APawn* PawnWithCorePawnComponent);

	UPROPERTY(BlueprintAssignable)
	FAbilityComponentInitializedMulticast OnAbilitySystemInitialized;

	virtual void Activate() override;

private:

	UPROPERTY()
	TObjectPtr<UCustomCorePawnComponent> CorePawnComponent;

	UFUNCTION()
	void AbilitySystemInitialized(UCustomAbilitySystemComponent* CustomASC);
};
