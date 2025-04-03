// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomStructs.generated.h"

USTRUCT(BlueprintType)
struct FMoveActorInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> ActorToMove = nullptr;
	UPROPERTY(BlueprintReadWrite)
	FVector TargetLocation = FVector(0);

	UPROPERTY()
	FVector FromLocation = FVector(0);
};

USTRUCT(BlueprintType)
struct FMoveActorSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	float Duration = 0.25f;
	// If true and moving Actor is a Character, then trace to ground to get accurate location
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTraceGroundIfCharacter = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAlphaBlendOption EasingFunc = EAlphaBlendOption::Linear;
	// Use this curve if EasingFunc is Custom
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "EasingFunc == EAlphaBlendOption::Custom", EditConditionHides))
	TObjectPtr<UCurveFloat> EasingCurve = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSweepCollisionWhileMoving = true;
	// If sweep collision, then fail and stop moving after a percentage of duration (0.2 means 20%)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f), meta = (EditCondition = "bSweepCollisionWhileMoving", EditConditionHides))
	float FailedProgressThreshold = 0.2f;
	// If NOT sweep collision and from 0.0 -> 1.0f, then enable sweep collision after this percentage duration (0.5 mean 50%)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "!bSweepCollisionWhileMoving", EditConditionHides))
	float ProgressToEnableCollision = -1.0f;
};