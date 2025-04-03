// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncActions/TickableAsyncAction.h"

#define UE_INLINE_GENERATED_CPP_BY_NAME(TickableAsyncAction)

void UTickableAsyncAction::Activate()
{
	Super::Activate();

	bActionActive = true;
}

void UTickableAsyncAction::Cancel()
{
	Super::Cancel();

	bActionActive = false;
}

TStatId UTickableAsyncAction::GetStatId() const
{
	return this->GetStatID();
}

bool UTickableAsyncAction::IsTickable() const
{
	return bActionActive;
}