// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncActions/TickableAsyncAction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TickableAsyncAction)

TStatId UTickableAsyncAction::GetStatId() const
{
	return this->GetStatID();
}

bool UTickableAsyncAction::IsTickable() const
{
	return IsActive();
}