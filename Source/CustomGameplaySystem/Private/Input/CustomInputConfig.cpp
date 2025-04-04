// Fill out your copyright notice in the Description page of Project Settings.

#include "Input/CustomInputConfig.h"
#include "CustomLogChannels.h"
#include "Misc/DataValidation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomInputConfig)

const UInputAction* UCustomInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	if (const FInputActionInfo* Action = NativeInputActions.Find(InputTag))
	{
		return Action->InputAction;
	}

	if (bLogNotFound)
	{
		UE_LOG(LogCustom, Error, TEXT("Can't find NativeInputActions for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* UCustomInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	if (const FInputActionInfo* Action = AbilityInputActions.Find(InputTag))
	{
		return Action->InputAction;
	}

	if (bLogNotFound)
	{
		UE_LOG(LogCustom, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

#if WITH_EDITOR
EDataValidationResult UCustomInputConfig::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	for (const TTuple<FGameplayTag, FInputActionInfo>& Action : NativeInputActions)
	{
		const FGameplayTag& InputTag = Action.Key;
		const FInputActionInfo& Info = Action.Value;

		if (!InputTag.IsValid())
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::FromString("Invalid Input Tag detected in NativeInputActions. Please find and assign a valid Input Tag."));
		}
		else if (!Info.InputAction)
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::FromString("Please assign Input Action for " + InputTag.ToString()));
		}
	}

	for (const TTuple<FGameplayTag, FInputActionInfo>& Action : AbilityInputActions)
	{
		const FGameplayTag& InputTag = Action.Key;
		const FInputActionInfo& Info = Action.Value;

		if (!InputTag.IsValid())
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::FromString("Invalid Input Tag detected in AbilityInputActions. Please find and assign a valid Input Tag."));
		}
		else if (!Info.InputAction)
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::FromString("Please assign Input Action for " + InputTag.ToString()));
		}
	}

	return Result;
}
#endif