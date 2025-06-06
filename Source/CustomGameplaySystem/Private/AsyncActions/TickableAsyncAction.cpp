// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncActions/TickableAsyncAction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TickableAsyncAction)

void UTickableAsyncAction::SetReadyToDestroy()
{
	Super::SetReadyToDestroy();

	// No game instance means IsActive() will return false
	RegisteredWithGameInstance.Reset();
}

TStatId UTickableAsyncAction::GetStatId() const
{
	return this->GetStatID();
}

bool UTickableAsyncAction::IsTickable() const
{
	return IsActive();
}

UWorld* UTickableAsyncAction::GetTickableGameObjectWorld() const
{
	// Provide a valid World so Tick can be paused when World is paused
	if (RegisteredWithGameInstance.Get())
	{
		return RegisteredWithGameInstance->GetWorld();
	}
	
	return FTickableGameObject::GetTickableGameObjectWorld();
}
