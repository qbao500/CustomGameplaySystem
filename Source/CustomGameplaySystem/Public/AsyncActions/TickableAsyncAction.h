// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/CancellableAsyncAction.h"
#include "TickableAsyncAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFinished);

/**
 * 
 */
UCLASS(Abstract)
class CUSTOMGAMEPLAYSYSTEM_API UTickableAsyncAction : public UCancellableAsyncAction, public FTickableGameObject
{
	GENERATED_BODY()

public:

	virtual void Activate() override;
	virtual void Cancel() override;

	//~ Begin FTickableGameObject interface
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;
	virtual void Tick(float DeltaTime) override {}
	//~ End FTickableGameObject interface
	
private:

	// Only tick when action is activated
	bool bActionActive = false;
	
};
