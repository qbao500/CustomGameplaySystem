// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/CustomHelperFunctionLibrary.h"

#include "Components/CapsuleComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/DamageEvents.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomHelperFunctionLibrary)

bool UCustomHelperFunctionLibrary::RandomWithChance(const float PercentChance)
{
	const int32 Random = FMath::RandRange(0, 100);
	return Random < PercentChance;
}

bool UCustomHelperFunctionLibrary::RandomWithChanceNormalize(const float NormalizedChance)
{
	const float Random = FMath::RandRange(0.0f, 1.0f);
	return Random < NormalizedChance;
}

bool UCustomHelperFunctionLibrary::IsTargetActorInFront(const AActor* MainActor, const AActor* TargetActor)
{
	if (!MainActor || !TargetActor) return false;

	const FVector Dir = GetUnitDirection2D(MainActor, TargetActor);
	const float Dot = FVector::DotProduct(Dir, MainActor->GetActorForwardVector().GetSafeNormal2D());
	return Dot >= 0;
}

bool UCustomHelperFunctionLibrary::IsTargetComponentInFront(const AActor* MainActor, const USceneComponent* TargetComponent)
{
	if (!MainActor || !TargetComponent) return false;

	const FVector Dir = GetUnitDirection2D(MainActor->GetActorLocation(), TargetComponent->GetComponentLocation());
	const float Dot = FVector::DotProduct(Dir, MainActor->GetActorForwardVector().GetSafeNormal2D());
	return Dot >= 0;
}

bool UCustomHelperFunctionLibrary::IsTargetLocationInFront(const AActor* MainActor, const FVector& TargetLocation)
{
	if (!MainActor) return false;

	const FVector Dir = GetUnitDirection2D(MainActor->GetActorLocation(), TargetLocation);
	const float Dot = FVector::DotProduct(Dir, MainActor->GetActorForwardVector().GetSafeNormal2D());
	return Dot >= 0;
}

float UCustomHelperFunctionLibrary::GetDotValueActor(const AActor* MainActor, const AActor* TargetActor)
{
	if (!MainActor  || !TargetActor) return 0.0f;
	
	const FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(MainActor->GetActorLocation(), TargetActor->GetActorLocation());
	const float Dot =  FVector::DotProduct(Direction, MainActor->GetActorForwardVector());
	return Dot;
}

float UCustomHelperFunctionLibrary::GetAngleBetweenDirections(const FVector& FirstDirection, const FVector& SecondDirection, const bool bUseDegree)
{
	const float Dot = FVector::DotProduct(FirstDirection, SecondDirection);
	
	// Degree
	if (bUseDegree) return FMath::RadiansToDegrees(FMath::Acos(Dot));

	// Radian
	return FMath::Acos(Dot);
}

FVector UCustomHelperFunctionLibrary::GetRandomDirection2D()
{
	FVector Direction = UKismetMathLibrary::RandomUnitVector();
	Direction.Z = 0;
	return Direction.GetSafeNormal2D();
}

FVector UCustomHelperFunctionLibrary::GetUnitDirection2D(const FVector& From, const FVector& To)
{
	return UKismetMathLibrary::GetDirectionUnitVector(FVector(From.X, From.Y, 0), FVector(To.X, To.Y, 0));
}

FVector UCustomHelperFunctionLibrary::GetUnitDirection2D(const AActor* From, const AActor* To)
{
	if (!From || !To) return FVector(0);
	
	const FVector2D NewFrom = FVector2D(From->GetActorLocation());
	const FVector2D NewTo = FVector2D(To->GetActorLocation());
	return UKismetMathLibrary::GetDirectionUnitVector(FVector(NewFrom, 0), FVector(NewTo, 0));
}

void UCustomHelperFunctionLibrary::GetPerpendicularVectors2D(const FVector& ForwardVector, FVector& OutLeftVector,
	FVector& OutRightVector)
{
	OutLeftVector = FVector(ForwardVector.Y, -ForwardVector.X, 0);
	OutRightVector = FVector(-ForwardVector.Y, ForwardVector.X, 0);
}

bool UCustomHelperFunctionLibrary::IsVectorALeftOfVectorB(const FVector& A, const FVector& B)
{
	// Calculate the cross-product between A and B
	const FVector CrossProduct = FVector::CrossProduct(A, B);

	// If the Z component of the cross-product is positive, vector A is to the left of vector B
	// If the Z component of the cross-product is negative, vector A is to the right of vector B
	return CrossProduct.Z > 0;
}

FRotator UCustomHelperFunctionLibrary::FindLookAtRotationActor(const AActor* Start, const AActor* Target)
{
	if (!Start || !Target) return FRotator(0);

	return UKismetMathLibrary::FindLookAtRotation(Start->GetActorLocation(), Target->GetActorLocation());
}

FVector UCustomHelperFunctionLibrary::GetLocationAheadActor(const AActor* Actor, const float Distance, const float AngleOffset)
{
	if (!Actor) return FVector(0);

	const FVector Dir = Actor->GetActorForwardVector().RotateAngleAxis(AngleOffset, FVector::UpVector);
	return Actor->GetActorLocation() + Dir * Distance;
}

TArray<FRotator> UCustomHelperFunctionLibrary::MakeSpreadRotators(const int32 NumRotators, float SpreadAngle,
	const FVector& Forward, const FVector& AroundAxis)
{
	TArray<FRotator> Rotators;
	
	if (NumRotators <= 0) return Rotators;

	// Make sure SpreadAngle is positive
	SpreadAngle = FMath::Abs(SpreadAngle);

	if (NumRotators > 1)
	{
		const FVector LeftSpread = Forward.RotateAngleAxis(-SpreadAngle / 2, AroundAxis);
		const float DeltaSpread = FMath::IsNearlyEqual(SpreadAngle, 360.0f) ? SpreadAngle / NumRotators : SpreadAngle / (NumRotators - 1);
		for (int32 I = 0; I < NumRotators; ++I)
		{
			const FVector Direction = LeftSpread.RotateAngleAxis(DeltaSpread * I, AroundAxis);
			Rotators.Emplace(Direction.Rotation());
		}
	}
	else
	{
		Rotators.Emplace(Forward.Rotation());
	}

	return Rotators;
}

TArray<FVector> UCustomHelperFunctionLibrary::MakeSpreadVectors(const int32 NumVectors, float SpreadAngle,
	const FVector& Forward, const FVector& AroundAxis)
{
	TArray<FVector> Vectors;
	
	if (NumVectors <= 0) return Vectors;

	// Make sure SpreadAngle is positive
	SpreadAngle = FMath::Abs(SpreadAngle);

	if (NumVectors > 1)
	{
		const FVector LeftSpread = Forward.RotateAngleAxis(-SpreadAngle / 2.0f, AroundAxis);
		const float DeltaSpread = FMath::IsNearlyEqual(SpreadAngle, 360.0f) ? SpreadAngle / NumVectors : SpreadAngle / (NumVectors - 1);
		for (int32 I = 0; I < NumVectors; ++I)
		{
			const FVector Direction = LeftSpread.RotateAngleAxis(DeltaSpread * I, AroundAxis);
			Vectors.Emplace(Direction);
		}
	}
	else
	{
		Vectors.Emplace(Forward);
	}

	return Vectors;
}

TArray<FVector> UCustomHelperFunctionLibrary::MakeRandomPointsAroundRadius2D(const FVector& CenterPoint,
	int32 NumPoints, const int32 MinRadius, const int32 MaxRadius, const bool bIncludeCenterPoint)
{
	TArray<FVector> OutPoints;

	if (NumPoints <= 0) return OutPoints;

	if (bIncludeCenterPoint)
	{
		OutPoints.Emplace(CenterPoint);
		NumPoints--;
	}

	// Check again if should continue
	if (NumPoints <= 0) return OutPoints;

	FVector Direction = GetRandomDirection2D();
	const float DeltaAngle = 360.0f / NumPoints;
	for (int32 I = 0; I < NumPoints; ++I)
	{
		// Still need a determined value
		Direction = Direction.RotateAngleAxis(DeltaAngle, FVector::UpVector);

		// 15% deviation for angle
		const int32 RandomPercentDeviation = FMath::RandRange(-15, 15);
		const float RandomAngleDeviation = DeltaAngle * (RandomPercentDeviation / 100.0f);
		const FVector RandomDir = Direction.RotateAngleAxis(RandomAngleDeviation, FVector::UpVector);
		
		const float RandomRadius = FMath::RandRange(MinRadius, MaxRadius);

		const FVector NewPoint = CenterPoint + RandomDir * RandomRadius;
		OutPoints.Emplace(NewPoint);
	}

	return OutPoints;
}

void UCustomHelperFunctionLibrary::TraceAndModifyPoints(const UObject* WorldContextObject, TArray<FVector>& Points, FCollisionProfileName TraceProfile, const float TotalTraceDistance)
{
	if (Points.Num() <= 0 || FMath::IsNearlyZero(TotalTraceDistance)) return;

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return;

	const float HalfDistance = FMath::Abs(TotalTraceDistance) / 2.0f;

	FHitResult HitResult;
	for (FVector& Point : Points)
	{
		const FVector Start = FVector(Point.X, Point.Y, Point.Z + HalfDistance);
		const FVector End = FVector(Point.X, Point.Y, Point.Z - HalfDistance);
		if (World->LineTraceSingleByProfile(HitResult, Start, End, TraceProfile.Name))
		{
			Point.Z = HitResult.ImpactPoint.Z;
		}
	}
}

void UCustomHelperFunctionLibrary::SetActorZ(AActor* InActor, const float InNewZ)
{
	if (!InActor) return;

	FVector Location = InActor->GetActorLocation();
	Location.Z = InNewZ;
	InActor->SetActorLocation(Location);
}

FVector UCustomHelperFunctionLibrary::GetCharacterFootLocation(const ACharacter* InCharacter)
{
	if (!InCharacter) return FVector();

	const UCapsuleComponent* Capsule = InCharacter->GetCapsuleComponent();

	return InCharacter->GetActorLocation() - FVector(0, 0, Capsule->GetScaledCapsuleHalfHeight());
}

FVector UCustomHelperFunctionLibrary::GetCharacterHeadLocation(const ACharacter* InCharacter)
{
	if (!InCharacter) return FVector();

	const UCapsuleComponent* Capsule = InCharacter->GetCapsuleComponent();

	return InCharacter->GetActorLocation() + FVector(0, 0, Capsule->GetScaledCapsuleHalfHeight());
}

FVector UCustomHelperFunctionLibrary::GetAttackLocation(const FVector& AttackerLocation, const FVector& TargetLocation,
	const float DistanceOffset, const float MaxDistance)
{
	const float Distance = FVector::Dist2D(AttackerLocation, TargetLocation);

	// If too close, then just stay there
	if (Distance <= DistanceOffset) return AttackerLocation;
		
	const FVector Dir = GetUnitDirection2D(TargetLocation, AttackerLocation);

	// Clamp to max distance if applicable
	if (MaxDistance > 0 && Distance > MaxDistance)
	{
		return AttackerLocation - Dir * MaxDistance;
	}
	
	return TargetLocation + Dir * DistanceOffset;
}

FVector UCustomHelperFunctionLibrary::GetAttackLocation(const AActor* AttackerActor, const AActor* TargetActor,
	const float DistanceOffset, const float MaxDistance)
{
	if (!AttackerActor || !TargetActor) return FVector(0);
	return GetAttackLocation(AttackerActor->GetActorLocation(), TargetActor->GetActorLocation(), DistanceOffset, MaxDistance);
}

FVector UCustomHelperFunctionLibrary::CalculateProjectileThrow(const FVector& ThrowPoint, const FVector& TargetPoint,
	float ProjectileGravity, float ThrowHeight)
{
	// Assuming that gravity goes down...
	ProjectileGravity = -FMath::Abs(ProjectileGravity);

	ThrowHeight += FMath::Max(TargetPoint.Z - ThrowPoint.Z, 0.0f);
	const float FallDownHeight = (ThrowPoint.Z + ThrowHeight) - TargetPoint.Z;
	const float ThrowUpTime = FMath::Sqrt((2.0 * ThrowHeight) / -ProjectileGravity);
	const float FallDownTime = FMath::Sqrt((2.0 * FallDownHeight) / -ProjectileGravity);
	const float HorSpeed = FVector::Dist2D(ThrowPoint, TargetPoint) / (ThrowUpTime + FallDownTime);
	const float VerSpeed = FMath::Sqrt(2.0 * -ProjectileGravity * ThrowHeight);

	FVector Throw = (TargetPoint - ThrowPoint);
	Throw.Z = 0;
	Throw = Throw.GetSafeNormal() * HorSpeed;
	Throw.Z = VerSpeed;
	return Throw;
}

bool UCustomHelperFunctionLibrary::IsTargetInRangeConeOfActor2D(const AActor* MainActor, const AActor* Target,
	const float CheckingDistance, const float HalfAngleDegree)
{
	if (!MainActor || !Target) return false;

	// Check distance first
	if (FVector::Dist2D(MainActor->GetActorLocation(), Target->GetActorLocation()) > CheckingDistance) return false;

	// Passed distance check. Return true if scanning 360
	if (HalfAngleDegree >= 180) return true;

	const float CurrentAngle = GetAngleBetweenDirections(MainActor->GetActorForwardVector(),
		GetUnitDirection2D(MainActor, Target));
	return CurrentAngle <= HalfAngleDegree;
}

void UCustomHelperFunctionLibrary::ClearCharacterMoveCompMovement(UCharacterMovementComponent* CharacterMovementComp)
{
	if (!CharacterMovementComp) return;

	CharacterMovementComp->StopActiveMovement();
	CharacterMovementComp->ClearAccumulatedForces();
	CharacterMovementComp->Velocity = FVector(0);
}

FVector UCustomHelperFunctionLibrary::FindNearestLocation(const FVector& CheckingLocation, const TArray<FVector>& LocationList)
{
	FVector BestLoc = CheckingLocation;
	float BestDist = FLT_MAX;
	for (const FVector& Loc : LocationList)
	{
		const float Distance = FVector::Dist(CheckingLocation, Loc);

		if (Distance < BestDist)
		{
			BestDist = Distance;
			BestLoc = Loc;
		}
	}

	return BestLoc;
}

AActor* UCustomHelperFunctionLibrary::FindNearestActor(const FVector& CheckingLocation, const TArray<AActor*>& ActorArray)
{
	AActor* BestActor = nullptr;
	float BestDist = FLT_MAX;
	for (AActor* Actor : ActorArray)
	{
		if (!Actor) continue;

		const FVector Loc = Actor->GetActorLocation();
		const float Distance = FVector::Dist(CheckingLocation, Loc);

		if (Distance < BestDist)
		{
			BestDist = Distance;
			BestActor = Actor;
		}
	}

	return BestActor;
}

AActor* UCustomHelperFunctionLibrary::FindNearestActor(const FVector& CheckingLocation,
	const TSet<TWeakObjectPtr<AActor>>& WeakActorSet)
{
	AActor* BestActor = nullptr;
	float BestDist = FLT_MAX;
	for (TWeakObjectPtr<AActor> WeakActor : WeakActorSet)
	{
		if (!WeakActor.Get()) continue;

		const FVector Loc = WeakActor->GetActorLocation();
		const float Distance = FVector::Dist(CheckingLocation, Loc);

		if (Distance < BestDist)
		{
			BestDist = Distance;
			BestActor = WeakActor.Get();
		}
	}

	return BestActor;
}

void UCustomHelperFunctionLibrary::SortActorsByDistance(const FVector& CheckingLocation, TArray<AActor*>& Actors)
{
	// Sort the actors in place by distance from the CheckingLocation using a lambda function
	Algo::Sort(Actors, [&CheckingLocation](const AActor* A, const AActor* B) -> bool
	{
		// Ensure actors are not null
		if (!A || !B)
		{
			return false;
		}

		// Calculate distance from CheckingLocation to Actor A and B
		const float DistA = FVector::Dist(CheckingLocation, A->GetActorLocation());
		const float DistB = FVector::Dist(CheckingLocation, B->GetActorLocation());
        
		// Compare the two distances
		return DistA < DistB;
	});
}

void UCustomHelperFunctionLibrary::SortActorsByDistance(const FVector& CheckingLocation,
	TArray<TWeakObjectPtr<AActor>>& WeakActors)
{
	// Sort the actors in place by distance from the CheckingLocation using a lambda function
	Algo::Sort(WeakActors, [&CheckingLocation](const TWeakObjectPtr<AActor> A, const TWeakObjectPtr<AActor> B) -> bool
	{
		// Ensure actors are not null
		if (!A.Get() || !B.Get())
		{
			return false;
		}

		// Calculate distance from CheckingLocation to Actor A and B
		const float DistA = FVector::Dist(CheckingLocation, A->GetActorLocation());
		const float DistB = FVector::Dist(CheckingLocation, B->GetActorLocation());
        
		// Compare the two distances
		return DistA < DistB;
	});
}

bool UCustomHelperFunctionLibrary::EqualHitActor(const FHitResult& HitA, const FHitResult& HitB)
{
	return HitA.GetActor() == HitB.GetActor();
}

TArray<AActor*> UCustomHelperFunctionLibrary::GetPlayerActors(const UObject* WorldContextObject)
{
	TArray<AActor*> Actors;
	if (AGameStateBase* GameState = UGameplayStatics::GetGameState(WorldContextObject))
	{
		for (const APlayerState* PlayerState : GameState->PlayerArray)
		{
			if (!PlayerState) continue;

			APawn* Pawn = PlayerState->GetPawn();
			if (!Pawn) continue;

			Actors.Emplace(Pawn);
		}
	}
	else if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		// Fallback if in Editor World, mostly used for EQS testing
		if (World->IsEditorWorld())
		{
			UGameplayStatics::GetAllActorsOfClass(WorldContextObject, APawn::StaticClass(), Actors);
		}
	}

	return Actors;
}

TArray<APawn*> UCustomHelperFunctionLibrary::GetPlayerPawns(const UObject* WorldContextObject)
{
	TArray<APawn*> Pawns;
	if (AGameStateBase* GameState = UGameplayStatics::GetGameState(WorldContextObject))
	{
		for (const APlayerState* PlayerState : GameState->PlayerArray)
		{
			if (!PlayerState) continue;

			APawn* Pawn = PlayerState->GetPawn();
			if (!Pawn) continue;

			Pawns.Emplace(Pawn);
		}
	}
	else if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		// Fallback if in Editor World, mostly used for EQS testing
		if (World->IsEditorWorld())
		{
			TArray<AActor*> Actors;
			UGameplayStatics::GetAllActorsOfClass(WorldContextObject, APawn::StaticClass(), Actors);
			for (AActor* Actor : Actors)
			{
				if (APawn* Pawn = Cast<APawn>(Actor))
				{
					Pawns.Emplace(Pawn);
				}
			}
		}
	}

	return Pawns;
}

bool UCustomHelperFunctionLibrary::ComponentIsDamageableFrom(const bool bForceDamageable, UPrimitiveComponent* VictimComp,
	const FVector& Origin, const AActor* IgnoredActor, const TArray<AActor*>& IgnoreActors, ECollisionChannel TraceChannel, FHitResult& OutHitResult)
{
	if (!bForceDamageable)
	{
		FCollisionQueryParams LineParams(SCENE_QUERY_STAT(ComponentIsVisibleFrom), true, IgnoredActor);
		LineParams.AddIgnoredActors( IgnoreActors );

		// Do a trace from origin to middle of box
		const UWorld* World = VictimComp->GetWorld();
		check(World);

		FVector const TraceEnd = VictimComp->Bounds.Origin;
		FVector TraceStart = Origin;
		if (Origin == TraceEnd)
		{
			// Tiny nudge so LineTraceSingle doesn't early out with no hits
			TraceStart.Z += 0.01f;
		}

		// Only do a line trace if there is a valid channel, if it is invalid then result will have no fall off
		if (TraceChannel != ECollisionChannel::ECC_MAX)
		{
			const bool bHadBlockingHit = World->LineTraceSingleByChannel(OutHitResult, TraceStart, TraceEnd, TraceChannel, LineParams);
			// DrawDebugLine(World, TraceStart, TraceEnd, FLinearColor::Red, true);

			// If there was a blocking hit, it will be the last one
			if (bHadBlockingHit)
			{
				if (OutHitResult.Component == VictimComp)
				{
					// if blocking hit was the victim component, it is visible
					return true;
				}
				else
				{
					// if we hit something else blocking, it's not
					UE_LOG(LogDamage, Log, TEXT("Radial Damage to %s blocked by %s (%s)"), *GetNameSafe(VictimComp), *OutHitResult.GetHitObjectHandle().GetName(), *GetNameSafe(OutHitResult.Component.Get()));
					return false;
				}
			}
		}
		else
		{
			UE_LOG(LogDamage, Warning, TEXT("ECollisionChannel::ECC_MAX is not valid! No falloff is added to damage"));
		}
	}

	// Didn't hit anything, assume nothing blocking the damage and victim is consequently visible
	// But since we don't have a hit result to pass back, construct a simple one, modeling the damage as having hit a point at the component's center.
	const FVector FakeHitLoc = VictimComp->GetComponentLocation();
	const FVector FakeHitNorm = (Origin - FakeHitLoc).GetSafeNormal();		// normal points back toward the epicenter
	OutHitResult = FHitResult(VictimComp->GetOwner(), VictimComp, FakeHitLoc, FakeHitNorm);
	
	return true;
}

bool UCustomHelperFunctionLibrary::IsPlayer(AActor* CheckingActor)
{
	if (!CheckingActor) return false;

	if (const APawn* Pawn = Cast<APawn>(CheckingActor))
	{
		if (Pawn->IsPlayerControlled())
		{
			return true;
		}
	}

	return false;
}

FVector UCustomHelperFunctionLibrary::ChooseBestAimingLocation(const FVector& AimingDirection, const FVector& AimingFromLocation,
                                                               const FVector& LocationA, const FVector& LocationB, bool& bPickedLocationA)
{
	const FVector DirA = UKismetMathLibrary::GetDirectionUnitVector(AimingFromLocation, LocationA);
	const FVector DirB = UKismetMathLibrary::GetDirectionUnitVector(AimingFromLocation, LocationB);

	const float DotA = FVector::DotProduct(AimingDirection, DirA);
	const float DotB = FVector::DotProduct(AimingDirection, DirB);
	
	if (DotA >= DotB)
	{
		bPickedLocationA = true;
		return LocationA;
	}
	else
	{
		bPickedLocationA = false;
		return LocationB;
	}
}

void UCustomHelperFunctionLibrary::AsyncLoad(const FSoftObjectPath& ItemToStream, const FStreamableDelegate& DelegateToCall)
{
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(ItemToStream, DelegateToCall);
}

void UCustomHelperFunctionLibrary::AsyncLoad(const TArray<FSoftObjectPath>& ItemsToStream, const FStreamableDelegate& DelegateToCall)
{
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(ItemsToStream, DelegateToCall);
}

float UCustomHelperFunctionLibrary::CalculateRadialDamage(const FRadialDamageEvent& RadialDamageEvent, FHitResult& OutHitResult)
{
	float ActualDamage = RadialDamageEvent.Params.BaseDamage;
	
	// find closest component
	// @todo, something more accurate here to account for size of components, e.g. closest point on the component bbox?
	// @todo, sum up damage contribution to each component?
	float ClosestHitDistSq = UE_MAX_FLT;
	for (int32 HitIdx = 0; HitIdx < RadialDamageEvent.ComponentHits.Num(); ++HitIdx)
	{
		FHitResult const& Hit = RadialDamageEvent.ComponentHits[HitIdx];
		float const DistSq = (Hit.ImpactPoint - RadialDamageEvent.Origin).SizeSquared();
		if (DistSq < ClosestHitDistSq)
		{
			OutHitResult = Hit;
			ClosestHitDistSq = DistSq;
		}
	}

	float const RadialDamageScale = RadialDamageEvent.Params.GetDamageScale( FMath::Sqrt(ClosestHitDistSq) );

	ActualDamage = FMath::Lerp(RadialDamageEvent.Params.MinimumDamage, ActualDamage, FMath::Max(0.f, RadialDamageScale));

	return ActualDamage;
}
