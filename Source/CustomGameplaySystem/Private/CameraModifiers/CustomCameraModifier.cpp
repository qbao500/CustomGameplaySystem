// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraModifiers/CustomCameraModifier.h"

bool FCameraModifierInfo::Equals(const FCameraModifierInfo& Other) const
{
	FCameraModifierInfo Delta;
	Delta.FOV = FOV - Other.FOV;
	Delta.SpringArmLength = SpringArmLength - Other.SpringArmLength;
	Delta.DepthOfFieldFocalDistance = DepthOfFieldFocalDistance - Other.DepthOfFieldFocalDistance;
	Delta.DepthOfFieldFocalRegion = DepthOfFieldFocalRegion - Other.DepthOfFieldFocalRegion;
	Delta.DepthOfFieldNearTransitionRegion = DepthOfFieldNearTransitionRegion - Other.DepthOfFieldNearTransitionRegion;
	Delta.DepthOfFieldFarTransitionRegion = DepthOfFieldFarTransitionRegion - Other.DepthOfFieldFarTransitionRegion;
	Delta.DepthOfFieldNearBlurSize = DepthOfFieldNearBlurSize - Other.DepthOfFieldNearBlurSize;
	Delta.DepthOfFieldFarBlurSize = DepthOfFieldFarBlurSize - Other.DepthOfFieldFarBlurSize;
	return Delta.IsZero();
}

bool FCameraModifierInfo::IsZero() const
{
	return FMath::Abs(FOV) < SMALL_NUMBER && FMath::Abs(SpringArmLength) < SMALL_NUMBER &&
		FMath::Abs(DepthOfFieldFocalDistance) < SMALL_NUMBER && FMath::Abs(DepthOfFieldFocalRegion) < SMALL_NUMBER &&
		FMath::Abs(DepthOfFieldNearTransitionRegion) < SMALL_NUMBER &&
		FMath::Abs(DepthOfFieldFarTransitionRegion) < SMALL_NUMBER &&
		FMath::Abs(DepthOfFieldNearBlurSize) < SMALL_NUMBER && FMath::Abs(DepthOfFieldFarBlurSize) < SMALL_NUMBER;
}

bool UCustomCameraModifier::ProcessViewRotation(AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	Super::ProcessViewRotation(ViewTarget, DeltaTime, OutViewRotation, OutDeltaRot);

	if (!IsValid(ViewTarget))
	{
		return false;
	}

	const APawn* Pawn = Cast<APawn>(ViewTarget);
	if (!IsValid(Pawn))
	{
		return false;
	}

	const APlayerController* PlayerController = Cast<APlayerController>(Pawn->Controller);
	if (!IsValid(PlayerController))
	{
		return false;
	}

	if (!PlayerController->RotationInput.IsNearlyZero(THRESH_QUAT_NORMALIZED))
	{
		// Reset cooldown.
		CooldownRemaining = CooldownAfterPlayerInput;
		return false;
	}

	if (CooldownRemaining > 0)
	{
		// Tick cooldown.
		CooldownRemaining -= DeltaTime;
		return false;
	}

	return false;
}

const FCameraModifierInfo& UCustomCameraModifier::GetCurrentModifiers() const
{
	return CurrentModifiers;
}

const FCameraModifierInfo& UCustomCameraModifier::GetTargetModifiers() const
{
	return TargetModifiers;
}

void UCustomCameraModifier::ApplyCameraTransition(const FCameraModifierInfo& CameraInfo, const float TransitionTime,
	FMinimalViewInfo& InOutPOV, const float DeltaTime)
{
	if (!TargetModifiers.Equals(CameraInfo))
    {
        CurrentModifiers = TargetModifiers;
        TargetModifiers = CameraInfo;

        if (RemainingTransitionTime > 0.0f)
        {
            // New transition started before old was finished. Play transition backwards without flipping.
            RemainingTransitionTime = TotalTransitionTime - RemainingTransitionTime;
        }
    }

    if (CurrentModifiers.Equals(TargetModifiers))
    {
        // No changes.
        ApplyCameraInfo(CurrentModifiers, 1.0f, InOutPOV);
        return;
    }

    // Three cases:
    // 1. CurrentModifiers is zero, TargetModifiers is not zero -> Apply new modifiers.
    // 2. CurrentModifiers is not zero, TargetModifiers is zero -> Stop applying modifiers.
    // 3. CurrentModifiers is not zero, TargetModifiers is not zero -> Need transition from current modifiers to new modifiers.
    // TODO(np): Handle Case 3: Need transition from current modifiers to new modifiers.

    // Apply modifiers.
    float ApplicationFactor = 0.0f;
    FCameraModifierInfo ModifiersToApply;

    if (!TargetModifiers.IsZero())
    {
        // Case 1: Apply new modifiers.
        ModifiersToApply = TargetModifiers;

        if (RemainingTransitionTime <= 0.0f)
        {
            // Start transition timer.
            TotalTransitionTime = TransitionTime;
            RemainingTransitionTime = TotalTransitionTime;
        }
        else
        {
            // Tick transition timer.
            RemainingTransitionTime -= DeltaTime;

        	
            if (RemainingTransitionTime <= 0.0f)
            {
                // Transition finished.
                CurrentModifiers = TargetModifiers;
                ApplicationFactor = 1.0f;
            }
            else
            {
                ApplicationFactor = 1 - (RemainingTransitionTime / TotalTransitionTime);
            }
        }
    }
    else
    {
        // Case 2: Stop applying modifiers.
        ModifiersToApply = CurrentModifiers;

        if (RemainingTransitionTime <= 0.0f)
        {
            // Restart transition timer.
            RemainingTransitionTime = TotalTransitionTime;
            ApplicationFactor = 1.0f;
        }
        else
        {
            // Tick transition timer.
            RemainingTransitionTime -= DeltaTime;

            if (RemainingTransitionTime <= 0.0f)
            {
                // Transition finished.
                CurrentModifiers = TargetModifiers;
                ApplicationFactor = 0.0f;
            }
            else
            {
                // This could as well be read as 1 - (1 - (RemainingTransitionTime / TotalTransitionTime)).
                // We want to gradually decrease the influence of the previous modifiers.
                ApplicationFactor = RemainingTransitionTime / TotalTransitionTime;
            }
        }
    }

    // Apply modifiers.
    ApplyCameraInfo(ModifiersToApply, ApplicationFactor, InOutPOV);
}

bool UCustomCameraModifier::PlayerHasRecentlyChangedCamera() const
{
	return CooldownRemaining > 0;
}

void UCustomCameraModifier::ApplyCameraInfo(const FCameraModifierInfo& CameraInfo, const float Factor,
	FMinimalViewInfo& InOutPOV) const
{
	const AActor* ViewTarget = GetViewTarget();
    if (!IsValid(ViewTarget))
    {
        return;
    }

    TArray<USceneComponent*> Children;
	ViewTarget->GetRootComponent()->GetChildrenComponents(true, Children);

    // Apply FOV.
	const float AppliedFOV = CameraInfo.FOV * Factor;
    InOutPOV.FOV = InOutPOV.FOV + AppliedFOV;

    if (!FMath::IsNearlyZero(CameraInfo.DepthOfFieldFocalDistance))
    {
        InOutPOV.PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
        InOutPOV.PostProcessSettings.DepthOfFieldFocalDistance += CameraInfo.DepthOfFieldFocalDistance * Factor;
    }
    
    if (!FMath::IsNearlyZero(CameraInfo.DepthOfFieldFocalRegion))
    {
        InOutPOV.PostProcessSettings.bOverride_DepthOfFieldFocalRegion = true;
        InOutPOV.PostProcessSettings.DepthOfFieldFocalRegion += CameraInfo.DepthOfFieldFocalRegion * Factor;
    }

    if (!FMath::IsNearlyZero(CameraInfo.DepthOfFieldNearTransitionRegion))
    {
        InOutPOV.PostProcessSettings.bOverride_DepthOfFieldNearTransitionRegion = true;
        InOutPOV.PostProcessSettings.DepthOfFieldNearTransitionRegion += CameraInfo.DepthOfFieldNearTransitionRegion * Factor;
    }

    if (!FMath::IsNearlyZero(CameraInfo.DepthOfFieldFarTransitionRegion))
    {
        InOutPOV.PostProcessSettings.bOverride_DepthOfFieldFarTransitionRegion = true;
        InOutPOV.PostProcessSettings.DepthOfFieldFarTransitionRegion += CameraInfo.DepthOfFieldFarTransitionRegion * Factor;
    }

    if (!FMath::IsNearlyZero(CameraInfo.DepthOfFieldNearBlurSize))
    {
        InOutPOV.PostProcessSettings.bOverride_DepthOfFieldNearBlurSize = true;
        InOutPOV.PostProcessSettings.DepthOfFieldNearBlurSize += CameraInfo.DepthOfFieldNearBlurSize * Factor;
    }

    if (!FMath::IsNearlyZero(CameraInfo.DepthOfFieldFarBlurSize))
    {
        InOutPOV.PostProcessSettings.bOverride_DepthOfFieldFarBlurSize = true;
        InOutPOV.PostProcessSettings.DepthOfFieldFarBlurSize += CameraInfo.DepthOfFieldFarBlurSize * Factor;
    }
}
