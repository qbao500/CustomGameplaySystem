// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CustomFloatingPawnMovement.h"

#include "FunctionLibraries/PrintLogFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomFloatingPawnMovement)

UCustomFloatingPawnMovement::UCustomFloatingPawnMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// For AI to have smoother movement with Acceleration.
	// Also, this will make the custom Launch() function work. 
	bUseAccelerationForPaths = true;
	
	MaxSpeed = 600.0f;
	Acceleration = 2048.0f;
	Deceleration = 2048.0f;
}

void UCustomFloatingPawnMovement::ApplyControlInputToVelocity(float DeltaTime)
{
	if (HandlePendingLaunch())
	{
		ConsumeInputVector();
		return;
	}
	
	Super::ApplyControlInputToVelocity(DeltaTime);
}

void UCustomFloatingPawnMovement::Launch(const FVector& LaunchVelocity)
{
	if (IsActive())
	{
		PendingLaunchVelocity = LaunchVelocity;
	}
}

bool UCustomFloatingPawnMovement::HandlePendingLaunch()
{
	if (!PendingLaunchVelocity.IsZero())
	{
		Velocity = PendingLaunchVelocity;
		PendingLaunchVelocity = FVector::ZeroVector;

		return true;
	}

	return false;
}
