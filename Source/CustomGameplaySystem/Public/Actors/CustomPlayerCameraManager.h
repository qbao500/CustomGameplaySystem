// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "CustomPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API ACustomPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	UCameraModifier* EnableCameraModifier(TSubclassOf<UCameraModifier> ModifierClass);
	UFUNCTION(BlueprintCallable)
	void DisableCameraModifier(TSubclassOf<UCameraModifier> ModifierClass, const bool bImmediate = false);
};
