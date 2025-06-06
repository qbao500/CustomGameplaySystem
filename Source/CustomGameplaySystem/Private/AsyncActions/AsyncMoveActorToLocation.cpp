// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncActions/AsyncMoveActorToLocation.h"

#include "Components/CapsuleComponent.h"
#include "FunctionLibraries/CustomHelperFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncMoveActorToLocation)

UAsyncMoveActorToLocation* UAsyncMoveActorToLocation::AsyncMoveActorToLocation(const FMoveActorInfo& MoveInfo, const FMoveActorSettings& MoveSettings)
{
	if (!MoveInfo.ActorToMove.Get()) return nullptr;

	// If somehow the settings have both of these values true, then don't do anything
	if (MoveSettings.bIgnoreXY && MoveSettings.bIgnoreZ)
	{
		return nullptr;
	}

	UAsyncMoveActorToLocation* Node = NewObject<UAsyncMoveActorToLocation>();
	
	// Cache MoveSettings
	Node->MoveSettings = MoveSettings;

	// Cache MoveInfo
	Node->MoveInfo = MoveInfo;
	Node->MoveInfo.FromLocation = MoveInfo.ActorToMove->GetActorLocation();
	if (const ACharacter* Character = Cast<ACharacter>(MoveInfo.ActorToMove))
	{
		Node->CharacterMoveComp = Character->GetCharacterMovement();
	}
	
	// Trace ground if applicable
	if (MoveSettings.bTraceGroundIfCharacter && Node->CharacterMoveComp.IsValid())
	{
		FHitResult HitResult;
		const FVector Start = MoveInfo.TargetLocation + FVector::UpVector * 50;
		const FVector End = MoveInfo.TargetLocation + FVector::UpVector * -200;
		FCollisionQueryParams QueryParams = FCollisionQueryParams::DefaultQueryParam;
		QueryParams.AddIgnoredActor(MoveInfo.ActorToMove.Get());
		if (MoveInfo.ActorToMove->GetWorld()->LineTraceSingleByObjectType(HitResult, Start, End, ECC_WorldStatic, QueryParams))
		{
			FVector NewTarget = HitResult.ImpactPoint;
			NewTarget.Z += Node->CharacterMoveComp->GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

			Node->MoveInfo.TargetLocation = NewTarget;
		}
	}

	// Register to not get Garbage Collection while running
	Node->RegisterWithGameInstance(MoveInfo.ActorToMove.Get());

	return Node;
}

void UAsyncMoveActorToLocation::Activate()
{
	if (!GetWorld()) return;

	if (CharacterMoveComp.IsValid())
	{
		HFL::ClearCharacterMoveCompMovement(CharacterMoveComp.Get());
	}

	Super::Activate();
}

void UAsyncMoveActorToLocation::Tick(float DeltaTime)
{
	AActor* ActorToMove = MoveInfo.ActorToMove.Get();
	if (!ActorToMove)
	{
		EndMove(true);
		return;
	}
	
	// No gravity
	if (CharacterMoveComp.IsValid())
	{
		CharacterMoveComp->Velocity.Z = 0.0f;
	}

	// Calculate current progress and location
	const float DeltaProgress = DeltaTime / MoveSettings.Duration / MoveInfo.ActorToMove->CustomTimeDilation;
	ProgressAlpha = FMath::Clamp(ProgressAlpha + DeltaProgress, 0.0f, 1.0f);
	const float MoveAlpha = FAlphaBlend::AlphaToBlendOption(ProgressAlpha, MoveSettings.EasingFunc, MoveSettings.CustomEasingCurve);

	// Calculate the new location
	FVector NewLocation = FMath::Lerp<FVector, float>(MoveInfo.FromLocation, MoveInfo.TargetLocation, MoveAlpha);
	if (MoveSettings.bIgnoreZ)
	{
		NewLocation.Z = ActorToMove->GetActorLocation().Z;
	}
	else if (MoveSettings.bIgnoreXY)
	{
		const FVector ActorLocation = ActorToMove->GetActorLocation();
		NewLocation.X = ActorLocation.X;
		NewLocation.Y = ActorLocation.Y;
	}

	const bool bSweep = MoveSettings.bSweepCollisionWhileMoving || (MoveSettings.ProgressToEnableCollision > 0.0f && ProgressAlpha >= MoveSettings.ProgressToEnableCollision);
	const bool bSuccess = ActorToMove->SetActorLocation(NewLocation, bSweep);

	if (bSuccess)
	{
		AccumulatedFailedProgress = 0;
	}
	else
	{
		// This means there's no more room for accumulating, early end move
		if (MoveSettings.FailedProgressThreshold - AccumulatedFailedProgress + ProgressAlpha > 1.0f)
		{
			//PLFL::PrintWarning(GetName() + " EndMove due to FailedProgressThreshold - AccumulatedFailedProgress + MoveProgress > 1.0f");
			EndMove(true);
			return;
		}
		
		ProgressAlpha -= DeltaProgress;
		AccumulatedFailedProgress += DeltaProgress;
		
		// This means the jump is continuously blocked until it reaches a certain threshold to end move
		if (AccumulatedFailedProgress >= MoveSettings.FailedProgressThreshold)
		{
			//PLFL::PrintWarning(GetName() + " EndMove due to AccumulatedFailedProgress >= FailedProgressThreshold");
			EndMove(true);
			return;
		}
	}

	if (ProgressAlpha >= 1.0f)
	{
		EndMove(false);
	}
}

float UAsyncMoveActorToLocation::GetCurrentProgressAlpha() const
{
	return ProgressAlpha;
}

void UAsyncMoveActorToLocation::EndMove(const bool bInterrupted)
{
	if (ShouldBroadcastDelegates())
	{
		OnMoveFinished.Broadcast(bInterrupted);
	}
	
	Cancel();
}

UWorld* UAsyncMoveActorToLocation::GetWorld() const
{
	return MoveInfo.ActorToMove.Get() ? MoveInfo.ActorToMove->GetWorld() : nullptr;
}