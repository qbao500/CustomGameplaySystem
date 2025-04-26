// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"
#include "ProjectileTargetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UProjectileTargetInterface : public UInterface
{
	GENERATED_BODY()
};

class CUSTOMGAMEPLAYSYSTEM_API IProjectileTargetInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetDefaultHomingInfo(USceneComponent*& HomingComponent, FName& HomingSocketName);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetWeakPointHomingInfo(USceneComponent*& HomingComponent, FName& HomingSocketName);
};
