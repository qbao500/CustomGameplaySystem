// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CustomHelperFunctionLibrary.generated.h"

#define HFL UCustomHelperFunctionLibrary

class UCharacterMovementComponent;

/**
 * A generic helper function library for various purposes, such as numbers, vectors, actors, etc.
 * If there are enough functions within a category, then it's (probably) recommended to create a new function library.
 */
UCLASS()
class CUSTOMGAMEPLAYLIBRARY_API UCustomHelperFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// From 0-100
	UFUNCTION(BlueprintPure)
	static bool RandomWithChance(float PercentChance);

	// From 0.0-1.0
	UFUNCTION(BlueprintPure)
	static bool RandomWithChanceNormalize(float NormalizedChance);

	// Direction vector and rotation
	UFUNCTION(BlueprintPure)
	static bool IsTargetActorInFront(const AActor* MainActor, const AActor* TargetActor);
	UFUNCTION(BlueprintPure)
	static bool IsTargetComponentInFront(const AActor* MainActor, const USceneComponent* TargetComponent);
	UFUNCTION(BlueprintPure)
	static bool IsTargetLocationInFront(const AActor* MainActor, const FVector& TargetLocation);
	UFUNCTION(BlueprintPure)
	static float GetDotValueActor(const AActor* MainActor, const AActor* TargetActor);
	UFUNCTION(BlueprintPure)
	static float GetAngleBetweenDirections(const FVector& FirstDirection, const FVector& SecondDirection, const bool bUseDegree = true);
	UFUNCTION(BlueprintPure)
	static FVector GetRandomDirection2D();
	UFUNCTION(BlueprintPure)
	static FVector GetUnitDirection2D(const FVector& From, const FVector& To);
	static FVector GetUnitDirection2D(const AActor* From, const AActor* To);
	UFUNCTION(BlueprintPure)
	static void GetPerpendicularVectors2D(const FVector& ForwardVector, FVector& OutLeftVector, FVector& OutRightVector);
	UFUNCTION(BlueprintPure)
	static bool IsVectorALeftOfVectorB(const FVector& A, const FVector& B);
	UFUNCTION(BlueprintPure)
	static FRotator FindLookAtRotationActor(const AActor* Start, const AActor* Target);
	UFUNCTION(BlueprintPure)
	static FVector GetLocationAheadActor(const AActor* Actor, const float Distance, const float AngleOffset = 0.0f);

	// Pattern maker
	UFUNCTION(BlueprintPure)
	static TArray<FRotator> MakeSpreadRotators(const int32 NumRotators, float SpreadAngle,
		const FVector& Forward, const FVector& AroundAxis = FVector::UpVector);
	UFUNCTION(BlueprintPure)
	static TArray<FVector> MakeSpreadVectors(const int32 NumVectors, float SpreadAngle,
		const FVector& Forward, const FVector& AroundAxis = FVector::UpVector);
	UFUNCTION(BlueprintPure)
	static TArray<FVector> MakeRandomPointsAroundRadius2D(const FVector& CenterPoint, int32 NumPoints, const int32 MinRadius,
		const int32 MaxRadius, const bool bIncludeCenterPoint = true);
	UFUNCTION(BlueprintCallable)
	static void TraceAndModifyPoints(const UObject* WorldContextObject,
		UPARAM(ref) TArray<FVector>& Points, FCollisionProfileName TraceProfile, const float TotalTraceDistance = 1000.0f);

	// Gameplay helper
	UFUNCTION(BlueprintCallable)
	static void SetActorZ(AActor* InActor, float InNewZ);
	UFUNCTION(BlueprintPure)
	static FVector GetCharacterFootLocation(const ACharacter* InCharacter);
	UFUNCTION(BlueprintPure)
	static FVector GetCharacterHeadLocation(const ACharacter* InCharacter);
	UFUNCTION(BlueprintPure)
	static FVector GetAttackLocation(const FVector& AttackerLocation, const FVector& TargetLocation, const float DistanceOffset, const float MaxDistance = -1);
	static FVector GetAttackLocation(const AActor* AttackerActor, const AActor* TargetActor, const float DistanceOffset, const float MaxDistance = -1);
	UFUNCTION(BlueprintPure)
	static FVector CalculateProjectileThrow(const FVector& ThrowPoint, const FVector& TargetPoint, float ProjectileGravity, float ThrowHeight);
	UFUNCTION(BlueprintPure)
	static bool IsTargetInRangeConeOfActor2D(const AActor* MainActor, const AActor* Target, const float CheckingDistance, const float HalfAngleDegree);
	UFUNCTION(BlueprintCallable)
	static void ClearCharacterMoveCompMovement(UCharacterMovementComponent* CharacterMovementComp);
	UFUNCTION(BlueprintPure)
	static FVector FindNearestLocation(const FVector& CheckingLocation, const TArray<FVector>& LocationList);
	UFUNCTION(BlueprintPure)
	static AActor* FindNearestActor(const FVector& CheckingLocation, const TArray<AActor*>& ActorArray);
	static AActor* FindNearestActor(const FVector& CheckingLocation, const TSet<TWeakObjectPtr<AActor>>& WeakActorSet);
	UFUNCTION(BlueprintCallable)
	static void SortActorsByDistance(const FVector& CheckingLocation, UPARAM(ref) TArray<AActor*>& Actors);
	static void SortActorsByDistance(const FVector& CheckingLocation, TArray<TWeakObjectPtr<AActor>>& WeakActors);
	UFUNCTION(BlueprintPure)
	static bool EqualHitActor(const FHitResult& HitA, const FHitResult& HitB);
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static TArray<AActor*> GetPlayerActors(const UObject* WorldContextObject);
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static TArray<APawn*> GetPlayerPawns(const UObject* WorldContextObject);
	UFUNCTION(BlueprintPure)
	static bool ComponentIsDamageableFrom(const bool bForceDamageable, UPrimitiveComponent* VictimComp, const FVector& Origin, const AActor* IgnoredActor,
		const TArray<AActor*>& IgnoreActors, ECollisionChannel TraceChannel, FHitResult& OutHitResult);
	UFUNCTION(BlueprintPure)
	static bool IsPlayer(AActor* CheckingActor);
	UFUNCTION(BlueprintCallable)
	static void SetMeshRagdoll(USkeletalMeshComponent* Mesh);

	// Projectile
	UFUNCTION(BlueprintPure)
	static FVector ChooseBestAimingLocation(const FVector& AimingDirection, const FVector& AimingFromLocation,
		const FVector& LocationA, const FVector& LocationB, bool& bPickedLocationA);

	// Helper for only C++
	static void AsyncLoad(const FSoftObjectPath& ItemToStream, const FStreamableDelegate& DelegateToCall = FStreamableDelegate());
	static void AsyncLoad(const TArray<FSoftObjectPath>& ItemsToStream, const FStreamableDelegate& DelegateToCall = FStreamableDelegate());

	static float CalculateRadialDamage(const FRadialDamageEvent& RadialDamageEvent, FHitResult& OutHitResult);
};
