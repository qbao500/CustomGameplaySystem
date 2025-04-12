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
	static UWaitForAbilitySystemInit* WaitForAbilityComponentInit(APawn* PawnWithCorePawnComponent);
	UFUNCTION(BlueprintCallable, Category = "Ability|AsyncAction", meta = (BlueprintInternalUseOnly = "true"))
	static UWaitForAbilitySystemInit* WaitForAbilityComponentInitWithCoreComp(UCustomCorePawnComponent* CorePawnComponent);

	UPROPERTY(BlueprintAssignable)
	FAbilityComponentInitialized OnAbilitySystemInitialized;

	virtual void Activate() override;

private:

	UPROPERTY()
	TObjectPtr<UCustomCorePawnComponent> CorePawnComp;

	UFUNCTION()
	void AbilitySystemInitialized(UCustomAbilitySystemComponent* CustomASC);
};
