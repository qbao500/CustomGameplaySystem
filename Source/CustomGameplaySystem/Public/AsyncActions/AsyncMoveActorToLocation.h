// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomStructs.h"
#include "TickableAsyncAction.h"
#include "AsyncMoveActorToLocation.generated.h"

class UCharacterMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMoveFinished, bool, bInterrupted);

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UAsyncMoveActorToLocation : public UTickableAsyncAction
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = true))
	static UAsyncMoveActorToLocation* AsyncMoveActorToLocation(const FMoveActorInfo& MoveInfo, const FMoveActorSettings& MoveSettings);
	
	UPROPERTY(BlueprintAssignable)
	FMoveFinished OnMoveFinished;

	virtual void Activate() override;
	virtual void Cancel() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure)
	float GetCurrentProgressAlpha() const;

protected:

	void EndMove(const bool bInterrupted);

	virtual UWorld* GetWorld() const override;

private:

	UPROPERTY(Transient)
	TWeakObjectPtr<UCharacterMovementComponent> CharacterMoveComp;
	FMoveActorInfo MoveInfo = FMoveActorInfo();
	FMoveActorSettings MoveSettings = FMoveActorSettings();

	float ProgressAlpha = 0.0f;
	float AccumulatedFailedProgress = 0.0f;
};
