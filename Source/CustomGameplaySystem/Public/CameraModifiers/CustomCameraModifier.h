// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraModifier.h"
#include "CustomCameraModifier.generated.h"

USTRUCT(BlueprintType)
struct FCameraModifierInfo
{
	GENERATED_BODY()

	/** The field of view (in degrees) in perspective mode (ignored in Orthographic mode) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float FOV = 0.0f;

	/** Distance between the camera and the view target, in cm. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float SpringArmLength = 0.0f;

	/** Distance in which the Depth of Field effect should be sharp, in unreal units (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DepthOfField)
	float DepthOfFieldFocalDistance = 0.0f;

	/** Artificial region where all content is in focus, starting after DepthOfFieldFocalDistance, in unreal units  (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DepthOfField)
	float DepthOfFieldFocalRegion = 0.0f;

	/** To define the width of the transition region next to the focal region on the near side (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DepthOfField)
	float DepthOfFieldNearTransitionRegion = 0.0f;

	/** To define the width of the transition region next to the focal region on the near side (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DepthOfField)
	float DepthOfFieldFarTransitionRegion = 0.0f;

	/** Gaussian only: Maximum size of the Depth of Field blur (in percent of the view width) (note: performance cost scales with size) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DepthOfField)
	float DepthOfFieldNearBlurSize = 0.0f;

	/** Gaussian only: Maximum size of the Depth of Field blur (in percent of the view width) (note: performance cost scales with size) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DepthOfField)
	float DepthOfFieldFarBlurSize = 0.0f;

	/** Compares both camera property sets for equality. */
	bool Equals(const FCameraModifierInfo& Other) const;

	/** Whether this property set modifies the camera at all. */
	bool IsZero() const;
};

/**
 * 
 */
UCLASS(Abstract)
class CUSTOMGAMEPLAYSYSTEM_API UCustomCameraModifier : public UCameraModifier
{
	GENERATED_BODY()

public:
	
	virtual bool ProcessViewRotation(AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;
	
	/** Gets the camera modifiers that are currently being applied. */
	const FCameraModifierInfo& GetCurrentModifiers() const;

	/** Gets the camera modifiers that we are currently transitioning to. */
	const FCameraModifierInfo& GetTargetModifiers() const;

	/** How long to wait after player has changed the camera before automatically changing it again, in seconds. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CooldownAfterPlayerInput;

protected:

	/**
	* Applies the specifies camera properties, starting a transition if required.
	*
	* @param CameraInfo Camera properties to apply. Will start a transition if the specified properties are different from the previous call. May be zero, in which case we'll just be transitioning back.
	* @param TransitionTime Time until transition to new camera properties should be finished, in seconds. You may pass zero if the transition started in an earlier frame.
	* @param InOutPOV Camera point of view being modified.
	* @param DeltaTime Time since the last application of this modifier, in seconds.
	*/
	void ApplyCameraTransition(const FCameraModifierInfo& CameraInfo, const float TransitionTime, FMinimalViewInfo& InOutPOV, const float DeltaTime);

	/** Whether the player has recently changed the camera. Check this in order to have their input always have priority over automatic adjustments. */
	bool PlayerHasRecentlyChangedCamera() const;

private:

	void ApplyCameraInfo(const FCameraModifierInfo& CameraInfo, const float Factor, FMinimalViewInfo& InOutPOV) const;
	
	FCameraModifierInfo CurrentModifiers = FCameraModifierInfo();
	FCameraModifierInfo TargetModifiers = FCameraModifierInfo();
	
	float TotalTransitionTime = 0.0f;
	float RemainingTransitionTime = 0.0f;

	/** Time before we automatically adjust the camera again, in seconds. */
	float CooldownRemaining = 0.0f;
};
