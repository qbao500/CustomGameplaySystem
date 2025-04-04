// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/CustomInputComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomInputComponent)

void UCustomInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (const uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
