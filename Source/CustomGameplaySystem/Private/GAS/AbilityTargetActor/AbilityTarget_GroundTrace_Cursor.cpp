// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilityTargetActor/AbilityTarget_GroundTrace_Cursor.h"

#include "Abilities/GameplayAbility.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AbilityTarget_GroundTrace_Cursor)

void AAbilityTarget_GroundTrace_Cursor::Tick(float DeltaSeconds)
{
	// very temp - do a mostly hardcoded trace from the source actor
	if (SourceActor && SourceActor->GetLocalRole() != ENetRole::ROLE_SimulatedProxy)
	{
		FHitResult HitResult = PerformTrace(SourceActor);
		FVector EndPoint = HitResult.Component.IsValid() ? HitResult.ImpactPoint : HitResult.TraceEnd;

#if ENABLE_DRAW_DEBUG
		if (bDebug)
		{
			DrawDebugLine(GetWorld(), SourceActor->GetActorLocation(), EndPoint, FColor::Green, false);
			DrawDebugSphere(GetWorld(), EndPoint, 16, 10, FColor::Green, false);
		}
#endif // ENABLE_DRAW_DEBUG

		if (bRotateWithAvatarActor)
		{
			SetActorLocationAndRotation(EndPoint, SourceActor->GetActorRotation());
		}
		else
		{
			SetActorLocation(EndPoint);
		}
	}

	AActor::Tick(DeltaSeconds);
}

FHitResult AAbilityTarget_GroundTrace_Cursor::PerformTrace(AActor* InSourceActor)
{
	constexpr bool bTraceComplex = false;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AAbilityTarget_GroundTrace_Cursor), bTraceComplex);
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActor(InSourceActor);

	FHitResult HitResult;
	TraceCursor(HitResult, InSourceActor->GetWorld(), Params);
	if (HitResult.IsValidBlockingHit())
	{
		bLastTraceWasGood = true;
	}

	return HitResult;
}

void AAbilityTarget_GroundTrace_Cursor::TraceCursor(FHitResult& OutHitResult, const UWorld* World,
	const FCollisionQueryParams& Params)
{
	if (!OwningAbility) // Server and launching client only
	{
		return;
	}

	APlayerController* PC = OwningAbility->GetCurrentActorInfo()->PlayerController.Get();
	check(PC);

	FVector2D ScreenPosition;
	PC->GetMousePosition(ScreenPosition.X, ScreenPosition.Y);

	FVector WorldOrigin;
	FVector WorldDirection;
	if (!UGameplayStatics::DeprojectScreenToWorld(PC, ScreenPosition, WorldOrigin, WorldDirection))
	{
		return;
	}

	World->LineTraceSingleByProfile(OutHitResult, WorldOrigin, WorldOrigin + WorldDirection * PC->HitResultTraceDistance, TraceProfile.Name, Params);
}
