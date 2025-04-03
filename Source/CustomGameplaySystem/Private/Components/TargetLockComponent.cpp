// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TargetLockComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "FunctionLibraries/CustomHelperFunctionLibrary.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "Interfaces/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystems/EnemyManagerSubsystem.h"

UTargetLockComponent::UTargetLockComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTargetLockComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;
	
	CameraManager = OwnerPawn->GetLocalViewingPlayerController()->PlayerCameraManager;
	EnemyManager = UWorld::GetSubsystem<UEnemyManagerSubsystem>(GetWorld());
}

void UTargetLockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!OwnerPawn || !EnemyManager) return;

	BestTarget = FindBestTarget();

	if (bDebug && BestTarget)
	{
		DrawDebugSphere(GetWorld(), BestTarget->GetActorLocation(), 25, 16, FColor::Purple, false, -1, 0, 3);
	}
}

AActor* UTargetLockComponent::GetBestTarget() const
{
	return BestTarget;
}

AActor* UTargetLockComponent::FindBestTarget() const
{
	AActor* ClosestTarget = nullptr;
	float BestScore = FLT_MAX; // We want to minimize the score

	// Get the player's location and forward direction
	const FVector PlayerLocation = OwnerPawn->GetActorLocation();
	const FVector CameraForward = OwnerPawn->GetControlRotation().Vector();

	for (AActor* Enemy : EnemyManager->GetNearbyEnemies())
	{
		if (!Enemy) continue; // Skip null pointers

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
		
		// Calculate the angle to the enemy
		FVector DirectionToTarget = (TargetLocation - PlayerLocation).GetSafeNormal();
		const float AngleToTarget = HFL::GetAngleBetweenDirections(CameraForward, DirectionToTarget);
		const float MaxAngle = bFinisherTarget ? 90.0f : MaxFocusAngle;

		// Ignore this enemy if it's not in camera focus
		if (AngleToTarget >= MaxAngle)
		{
			DebugString(Enemy, "Out of focus", 0, 1.5f, FColor::Yellow);
			continue;
		}
		
		// Check sight, ignore if blocked
		if (IsSightToTargetBlocked(Enemy))
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
		DebugString(Enemy, "Score: " + FString::FromInt(Score), 20);
		DebugString(Enemy, "Distance: " + FString::FromInt(Distance), 0);
		DebugString(Enemy, "Angle: " + FString::Printf(TEXT("%.2f"), AngleToTarget), -20);
		
		// Find the best (the lowest score) enemy
		if (Score < BestScore)
		{
			BestScore = Score;
			ClosestTarget = Enemy;
		}
	}

	return ClosestTarget;
}

bool UTargetLockComponent::IsSightToTargetBlocked(const AActor* Target) const
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerPawn);
	Params.AddIgnoredActor(Target);
	return GetWorld()->LineTraceSingleByChannel(Hit, CameraManager->GetCameraLocation(), Target->GetActorLocation(),
		ECC_Visibility, Params);
}

float UTargetLockComponent::GetOnAirMultiplier() const
{
	if (!bCheckOnAir) return 1.0f;

	if (!UKismetSystemLibrary::DoesImplementInterface(OwnerPawn, UCombatInterface::StaticClass())) return 1.0f;

	return ICombatInterface::Execute_IsOnAir(OwnerPawn) ? OnAirMaxDistanceMultiplier : 1.0f;
}

void UTargetLockComponent::DebugString(AActor* Actor, const FString& Text, const float HeightOffset, const float FontScale, const FColor Color) const
{
	if (!Actor || !bDebug) return;

	DrawDebugString(Actor->GetWorld(), FVector(0, 0, HeightOffset), Text, Actor, Color, 0, false, FontScale);
}
