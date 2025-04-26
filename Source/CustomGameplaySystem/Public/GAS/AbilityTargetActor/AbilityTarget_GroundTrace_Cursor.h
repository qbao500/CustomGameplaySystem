// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbilityTargetActor_GroundTrace.h"
#include "AbilityTarget_GroundTrace_Cursor.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API AAbilityTarget_GroundTrace_Cursor : public AGameplayAbilityTargetActor_GroundTrace
{
	GENERATED_BODY()

public:

	virtual void Tick(float DeltaSeconds) override;

protected:

	virtual FHitResult PerformTrace(AActor* InSourceActor) override;

	virtual void TraceCursor(FHitResult& OutHitResult, const UWorld* World, const FCollisionQueryParams& Params);

private:

	UPROPERTY(EditDefaultsOnly, Category = "Rotation")
	bool bRotateWithAvatarActor = false;
};
