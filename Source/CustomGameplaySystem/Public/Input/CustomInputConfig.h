// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "CustomInputConfig.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FInputActionInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction;
};

/**
 * Non-mutable data asset that contains input configuration properties.
 */
UCLASS(BlueprintType, Const)
class CUSTOMGAMEPLAYSYSTEM_API UCustomInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:	

	UFUNCTION(BlueprintCallable, meta = (Categories = "Input", AutoCreateRefTerm = "InputTag"))
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	UFUNCTION(BlueprintCallable, meta = (Categories = "Input", AutoCreateRefTerm = "InputTag"))
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;
	
	// List of input actions used by the owner. These input actions are mapped to a gameplay tag and must be manually bound.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Input", ForceInlineRow))
	TMap<FGameplayTag, FInputActionInfo> NativeInputActions;

	// List of input actions used by the owner. These input actions are mapped to a gameplay tag and are automatically bound to abilities with matching input tags.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Input", ForceInlineRow))
	TMap<FGameplayTag, FInputActionInfo> AbilityInputActions;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
