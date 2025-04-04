// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "CustomInputConfig.h"
#include "CustomInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UCustomInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	template<class UserClass, typename FuncType>
	void BindNativeAction(const UCustomInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent,
		UserClass* Object, FuncType Func, bool bLogIfNotFound);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	TArray<uint32> BindAbilityActions(const UCustomInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc,
		ReleasedFuncType ReleasedFunc);

	void RemoveBinds(TArray<uint32>& BindHandles);
};

template <class UserClass, typename FuncType>
void UCustomInputComponent::BindNativeAction(const UCustomInputConfig* InputConfig, const FGameplayTag& InputTag,
	ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);
	
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
TArray<uint32> UCustomInputComponent::BindAbilityActions(const UCustomInputConfig* InputConfig, UserClass* Object,
	PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc)
{
	check(InputConfig);

	TArray<uint32> BindHandles;
	for (const TTuple<FGameplayTag, FInputActionInfo>& Action : InputConfig->AbilityInputActions)
	{
		const FGameplayTag& InputTag = Action.Key;
		const FInputActionInfo& Info = Action.Value;
		
		if (Info.InputAction && InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(Info.InputAction, ETriggerEvent::Started, Object, PressedFunc, InputTag).GetHandle());
			}
			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(Info.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, InputTag).GetHandle());
			}
		}
	}

	return BindHandles;
}
