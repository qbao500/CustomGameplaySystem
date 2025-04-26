// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AsyncActions/TickableAsyncAction.h"
#include "AsyncRotateActorToTarget.generated.h"

USTRUCT()
struct FRotateActorInfo
{
	GENERATED_BODY()

	TWeakObjectPtr<AActor> ActorToRotate;
	FQuat FromQuat = FQuat();
	FQuat ToQuat = FQuat();
};

USTRUCT(BlueprintType)
struct FRotateActorSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	float Duration = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAlphaBlendOption EasingFunc = EAlphaBlendOption::Linear;
	// Use this curve if EasingFunc is Custom
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "EasingFunc == EAlphaBlendOption::Custom", EditConditionHides))
	TObjectPtr<UCurveFloat> EasingCurve = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIgnoreTargetYaw = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIgnoreTargetPitch = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIgnoreTargetRoll = true;

	void ModifyRotator(FRotator& Rotator) const;
};

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UAsyncRotateActorToTarget : public UTickableAsyncAction
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = true))
	static UAsyncRotateActorToTarget* AsyncRotateActorToActor(AActor* ActorToRotate, AActor* TargetActor, const FRotateActorSettings& RotateSettings);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = true))
	static UAsyncRotateActorToTarget* AsyncRotateActorToLocation(AActor* ActorToRotate, const FVector& TargetLocation, const FRotateActorSettings& RotateSettings);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = true))
	static UAsyncRotateActorToTarget* AsyncRotateActorToDirection(AActor* ActorToRotate, const FVector& TargetDirection, const FRotateActorSettings& RotateSettings);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = true))
	static UAsyncRotateActorToTarget* AsyncRotateActorToRotation(AActor* ActorToRotate, const FRotator& TargetRotation, const FRotateActorSettings& RotateSettings);
	
	UPROPERTY(BlueprintAssignable)
	FFinished OnRotateFinished;

	virtual void Cancel() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure)
	float GetCurrentProgressAlpha() const;

protected:

	virtual UWorld* GetWorld() const override;

private:

	FRotateActorInfo RotateInfo = FRotateActorInfo();
	FRotateActorSettings RotateSettings = FRotateActorSettings();

	float ProgressAlpha = 0.0f;

	static UAsyncRotateActorToTarget* SpawnNodeCommon(AActor* ActorToRotate, const FRotateActorSettings& RotateSettings);
	
	void SetTargetQuat(FRotator& Rotator);
};
