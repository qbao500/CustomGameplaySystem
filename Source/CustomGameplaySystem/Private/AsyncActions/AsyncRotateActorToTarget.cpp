// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncActions/AsyncRotateActorToTarget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncRotateActorToTarget)

void FRotateActorSettings::ModifyRotator(FRotator& Rotator) const
{
	if (bIgnoreTargetYaw)
	{
		Rotator.Yaw = 0.0f;
	}
	if (bIgnoreTargetPitch)
	{
		Rotator.Pitch = 0.0f;
	}
	if (bIgnoreTargetRoll)
	{
		Rotator.Roll = 0.0f;
	}
}

UAsyncRotateActorToTarget* UAsyncRotateActorToTarget::AsyncRotateActorToActor(AActor* ActorToRotate,
	AActor* TargetActor, const FRotateActorSettings& RotateSettings)
{
	if (!ActorToRotate || !TargetActor) return nullptr;

	UAsyncRotateActorToTarget* Node = SpawnNodeCommon(ActorToRotate, RotateSettings);
	if (!Node) return nullptr;

	// Calculate rotator
	FRotator RotationToTarget = (TargetActor->GetActorLocation() - ActorToRotate->GetActorLocation()).GetSafeNormal().ToOrientationRotator();
	Node->SetTargetQuat(RotationToTarget);

	Node->RegisterWithGameInstance(ActorToRotate);
	
	return Node;
}

UAsyncRotateActorToTarget* UAsyncRotateActorToTarget::AsyncRotateActorToLocation(AActor* ActorToRotate,
	const FVector& TargetLocation, const FRotateActorSettings& RotateSettings)
{
	UAsyncRotateActorToTarget* Node = SpawnNodeCommon(ActorToRotate, RotateSettings);
	if (!Node) return nullptr;

	// Calculate rotator
	FRotator RotationToTarget = (TargetLocation - ActorToRotate->GetActorLocation()).GetSafeNormal().ToOrientationRotator();
	Node->SetTargetQuat(RotationToTarget);
	
	Node->RegisterWithGameInstance(ActorToRotate);
	
	return Node;
}

UAsyncRotateActorToTarget* UAsyncRotateActorToTarget::AsyncRotateActorToDirection(AActor* ActorToRotate,
	const FVector& TargetDirection, const FRotateActorSettings& RotateSettings)
{
	UAsyncRotateActorToTarget* Node = SpawnNodeCommon(ActorToRotate, RotateSettings);
	if (!Node) return nullptr;

	// Calculate rotator
	FRotator RotationToTarget = TargetDirection.GetSafeNormal().ToOrientationRotator();
	Node->SetTargetQuat(RotationToTarget);

	Node->RegisterWithGameInstance(ActorToRotate);
	
	return Node;
}

UAsyncRotateActorToTarget* UAsyncRotateActorToTarget::AsyncRotateActorToRotation(AActor* ActorToRotate,
	const FRotator& TargetRotation, const FRotateActorSettings& RotateSettings)
{
	UAsyncRotateActorToTarget* Node = SpawnNodeCommon(ActorToRotate, RotateSettings);
	if (!Node) return nullptr;

	// Calculate rotator
	FRotator RotationToTarget = TargetRotation;
	Node->SetTargetQuat(RotationToTarget);

	Node->RegisterWithGameInstance(ActorToRotate);
	
	return Node;
}

void UAsyncRotateActorToTarget::Cancel()
{
	Super::Cancel();

	OnRotateFinished.Broadcast();
}

void UAsyncRotateActorToTarget::Tick(float DeltaTime)
{
	if (RotateInfo.ActorToRotate.Get() == nullptr)
	{
		Cancel();
		return;
	}
	
	// Calculate current progress and quat
	const float DeltaProgress = DeltaTime / RotateSettings.Duration / RotateInfo.ActorToRotate->CustomTimeDilation;
	ProgressAlpha = FMath::Clamp(ProgressAlpha + DeltaProgress, 0.0f, 1.0f);
	const float RotateAlpha = FAlphaBlend::AlphaToBlendOption(ProgressAlpha, RotateSettings.EasingFunc, RotateSettings.EasingCurve);
	const FQuat Quat = FMath::Lerp<FQuat, float>(RotateInfo.FromQuat, RotateInfo.ToQuat, RotateAlpha);

	RotateInfo.ActorToRotate->SetActorRotation(Quat);

	if (ProgressAlpha >= 1.0f)
	{
		Cancel();
	}
}

float UAsyncRotateActorToTarget::GetCurrentProgressAlpha() const
{
	return ProgressAlpha;
}

UWorld* UAsyncRotateActorToTarget::GetWorld() const
{
	return RotateInfo.ActorToRotate.Get() ? RotateInfo.ActorToRotate->GetWorld() : nullptr;
}

UAsyncRotateActorToTarget* UAsyncRotateActorToTarget::SpawnNodeCommon(AActor* ActorToRotate,
	const FRotateActorSettings& RotateSettings)
{
	if (!ActorToRotate) return nullptr;

	// If for some reason it's ignoring all, then don't rotate
	if (RotateSettings.bIgnoreTargetYaw && RotateSettings.bIgnoreTargetPitch && RotateSettings.bIgnoreTargetRoll) return nullptr;

	UAsyncRotateActorToTarget* Node = NewObject<UAsyncRotateActorToTarget>();

	// Register to not get Garbage Collection while running
	Node->RegisterWithGameInstance(ActorToRotate);

	// Cache RotateSettings
	Node->RotateSettings = RotateSettings;

	// Setup RotateInfo
	FRotateActorInfo& Info = Node->RotateInfo;
	Info.ActorToRotate = ActorToRotate;
	Info.FromQuat = ActorToRotate->GetActorQuat();

	return Node;
}

void UAsyncRotateActorToTarget::SetTargetQuat(FRotator& Rotator)
{
	RotateSettings.ModifyRotator(Rotator);

	RotateInfo.ToQuat = Rotator.Quaternion();
}
