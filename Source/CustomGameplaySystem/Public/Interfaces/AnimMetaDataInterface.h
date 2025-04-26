// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "AnimMetaDataInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAnimMetaDataInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *	Required AnimInstance to inherit CustomAnimInstance, and enable certain options corresponding to the event you want to override
 */
class CUSTOMGAMEPLAYSYSTEM_API IAnimMetaDataInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Anim Meta Data")
	void OnMontageStarted(UAnimMontage* Montage, USkeletalMeshComponent* MeshComponent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Anim Meta Data")
	FGameplayTagContainer GetCurrentTags() const;
};
