// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AsyncActions/AsyncRotateActorToTarget.h"
#include "ANS_RotateActorToTarget.generated.h"

class UAsyncRotateActorToTarget;

UENUM(BlueprintType)
enum class ERotateActorMode : uint8
{
	// Rotate to Combat Target. Lock the direction to target when this Notify begins (NO update if Target moves).
	// Require owner actor to implement ICombatInterface and override GetCombatTarget()
	CombatTarget				UMETA(DisplayName = "Combat Target"),
	// Rotate to Combat Target and always track it. Use Rotation Speed. EasingFunc in RotateSettings doesn't have effect.
	// Require owner actor to implement ICombatInterface and override GetCombatTarget()
	CombatTargetTracking		UMETA(DisplayName = "Tracking Combat Target"),
	SpecifiedRotation,
};

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UANS_RotateActorToTarget : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif
	
protected:

	UPROPERTY(EditAnywhere)
	FRotateActorSettings RotateSettings = FRotateActorSettings();

	UPROPERTY(EditAnywhere)
	ERotateActorMode RotateMode = ERotateActorMode::CombatTarget;

	UPROPERTY(EditAnywhere, meta = (EditCondition, EditConditionHides))
	float RotationSpeed = 5.0f;

	// Specified Rotation
	UPROPERTY(EditAnywhere, meta = (EditCondition, EditConditionHides))
	FRotator Rotation = FRotator(0);

	// If still rotating when animation ends (usually animation is cancelled early), then also stop if true
	UPROPERTY(EditAnywhere)
	bool bStopRotateWhenAnimationEnded = true;

private:

	TWeakObjectPtr<UAsyncRotateActorToTarget> Action;
	
	TWeakObjectPtr<AActor> TrackingActor;
	bool bTrackingNeedToBypassRateOptimization = false;
};
