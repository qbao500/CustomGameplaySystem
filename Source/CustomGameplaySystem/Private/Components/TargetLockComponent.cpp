// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TargetLockComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "FunctionLibraries/CustomHelperFunctionLibrary.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "Interfaces/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystems/EnemyManagerSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TargetLockComponent)

UTargetLockComponent::UTargetLockComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;
}

void UTargetLockComponent::BeginPlay()
{
	Super::BeginPlay();

	// If debug is on, then force to tick every frame
#if WITH_EDITOR
	if (bDebug)
	{
		SetComponentTickInterval(-1.0f);
	}
#endif

	OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;
	
	CameraManager = OwnerPawn->GetLocalViewingPlayerController()->PlayerCameraManager;
	EnemyManager = UWorld::GetSubsystem<UEnemyManagerSubsystem>(GetWorld());
}

void UTargetLockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TickFindTarget(DeltaTime);
}

AActor* UTargetLockComponent::GetBestTarget() const
{
	return BestTarget.Get();
}

float UTargetLockComponent::GetMaxDistance() const
{
	return MaxDistance;
}

float UTargetLockComponent::GetLocationForwardOffset() const
{
	return LocationForwardOffset;
}

void UTargetLockComponent::SetMaxFocusAngle(const float NewValue)
{
	MaxFocusAngle = FMath::Abs(NewValue);
}

void UTargetLockComponent::TickFindTarget(float DeltaTime)
{
	if (!OwnerPawn || !EnemyManager) return;

	BestTarget = FindBestTarget();
	if (BestTarget.Get())
	{
		OnFoundBestTarget.Broadcast(BestTarget.Get());
	}

	if (bDebug && BestTarget.Get())
	{
		DrawDebugSphere(GetWorld(), BestTarget->GetActorLocation(), 25, 16, FColor::Purple,
			false, GetDebugTime(), 0, 3);
	}
}

AActor* UTargetLockComponent::FindBestTarget(const TSet<AActor*>& IgnoredActors) const
{
	AActor* BestActor = nullptr;
	float BestScore = FLT_MAX; // We want to minimize the score

	// Get the player's location and forward direction
	const FVector PlayerLocation = GetOwnerLocation();
	const FVector CameraForward = OwnerPawn->GetControlRotation().Vector();

	TSet<AActor*> NearbyEnemies = EnemyManager->GetNearbyEnemies();
	for (AActor* Enemy : NearbyEnemies)
	{
		if (!IsValid(Enemy)) continue;

		if (IgnoredActors.Contains(Enemy)) continue;

		bool bFinisherTarget = false;
		if (bCheckFinisherTarget && FinisherTag.IsValid())
		{
			if (const UAbilitySystemComponent* AbilityComp = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Enemy))
			{
				if (AbilityComp->GetOwnedGameplayTags().HasTag(FinisherTag))
				{
					bFinisherTarget = true;
				}
			}
		}

		// Calculate the distance to the enemy
		FVector TargetLocation = Enemy->GetActorLocation();
		const float Distance = FVector::Dist(PlayerLocation, TargetLocation);
		const float FinalMaxDistance = MaxDistance * GetOnAirMultiplier() * (bFinisherTarget ? FinisherMaxDistanceMultiplier : 1.0f);
		if (Distance > FinalMaxDistance)
		{
			DebugString(Enemy, "Too far", 0, 1.5f, FColor::Yellow);
			continue;
		}
		
		// Calculate the angle to the enemy, if method is FromCamera
		float AngleToTarget = 0.0f;
		if (ShouldCheckLookAngle())
		{
			FVector DirectionToTarget = (TargetLocation - PlayerLocation).GetSafeNormal();
			AngleToTarget = HFL::GetAngleBetweenDirections(CameraForward, DirectionToTarget);
			const float MaxAngle = bFinisherTarget ? 90.0f : MaxFocusAngle;

			// Ignore this enemy if it's not in camera focus
			if (AngleToTarget >= MaxAngle)
			{
				DebugString(Enemy, "Out of focus", 0, 1.5f, FColor::Yellow);
				continue;
			}
		}
		
		// Check sight, ignore if blocked
		if (bCheckLineOfSight && IsSightToTargetBlocked(Enemy))
		{
			DebugString(Enemy, "Sight Blocked", 0, 1.5f, FColor::Red);
			continue;
		}
		
		// Combine distance and angle into a single score.
		// We might want to prioritize closer targets over ones in the field of view.
		// For example, distance can be weighted more heavily than an angle.
		float Score = Distance + (AngleToTarget * AngleScoreMultiplier);
		
		if (bFinisherTarget)
		{
			Score /= FinisherScoreMultiplier;
		}

		// Debug score
		DebugString(Enemy, "Score: " + FString::FromInt(Score), DebugStringHeightOffset, 1.35f, FColor::White);
		if (!bDebugOnyScore && ShouldCheckLookAngle())
		{
			constexpr FColor SubColor (255, 255, 255, 200);
			DebugString(Enemy, "Distance: " + FString::FromInt(Distance), 0, 1.25f, SubColor);
			DebugString(Enemy, "Angle: " + FString::Printf(TEXT("%.2f"), AngleToTarget), -DebugStringHeightOffset, 1.25f, SubColor);
		}
		
		// Find the best (the lowest score) enemy
		if (Score < BestScore)
		{
			BestScore = Score;
			BestActor = Enemy;
		}
	}

	return BestActor;
}

bool UTargetLockComponent::ShouldCheckLookAngle() const
{
	switch (TargetLockMethod)
	{
	case ETargetLockMethod::FromCamera:
	case ETargetLockMethod::FromOwnerPawn:
		return true;
	default: 
		return false;
	}
}

bool UTargetLockComponent::IsSightToTargetBlocked(const AActor* Target) const
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerPawn);
	Params.AddIgnoredActor(Target);
	const FVector StartLoc = TargetLockMethod == ETargetLockMethod::FromCamera ? CameraManager->GetCameraLocation() : GetOwnerLocation();
	
	return GetWorld()->LineTraceSingleByChannel(Hit, StartLoc, Target->GetActorLocation(), LineOfSightChannel, Params);
}

float UTargetLockComponent::GetOnAirMultiplier() const
{
	if (!bCheckOnAir) return 1.0f;

	if (!UKismetSystemLibrary::DoesImplementInterface(OwnerPawn, UCombatInterface::StaticClass())) return 1.0f;

	return ICombatInterface::Execute_IsOnAir(OwnerPawn) ? OnAirMaxDistanceMultiplier : 1.0f;
}

FVector UTargetLockComponent::GetOwnerLocation() const
{
	check(OwnerPawn);
	return OwnerPawn->GetActorLocation() + (OwnerPawn->GetActorForwardVector() * LocationForwardOffset);
}

void UTargetLockComponent::DebugString(AActor* Actor, const FString& Text, const float HeightOffset, const float FontScale, const FColor Color) const
{
	if (!Actor || !bDebug) return;

	DrawDebugString(Actor->GetWorld(), FVector(0, 0, HeightOffset), Text, Actor, Color, 0, false, FontScale);
}

float UTargetLockComponent::GetDebugTime() const
{
	return GetComponentTickInterval();
}
