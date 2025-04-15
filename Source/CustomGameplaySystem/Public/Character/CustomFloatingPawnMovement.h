// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "CustomFloatingPawnMovement.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UCustomFloatingPawnMovement : public UFloatingPawnMovement
{
	GENERATED_BODY()

public:

	UCustomFloatingPawnMovement(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ Begin UFloatingPawnMovement interface
	virtual void ApplyControlInputToVelocity(float DeltaTime) override;
	//~ End UFloatingPawnMovement interface

	UFUNCTION(BlueprintCallable)
	void Launch(const FVector& LaunchVelocity);

private:

	bool HandlePendingLaunch();
	FVector PendingLaunchVelocity = FVector::ZeroVector;
};
