// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CustomProjectileComponent.h"

#include "FunctionLibraries/CustomHelperFunctionLibrary.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "Interfaces/ProjectileTargetInterface.h"
#include "Kismet/KismetSystemLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomProjectileComponent)

UCustomProjectileComponent::UCustomProjectileComponent()
{
	bAutoActivate = false;
	bRotationFollowsVelocity = true;
	bInitialVelocityInLocalSpace = false;
	ProjectileGravityScale = 0.2f;
	MaxSpeed = 2000.0f;
	HomingAccelerationMagnitude = 3000.0f;
}

void UCustomProjectileComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsBoomerang() && GetOwner())
	{
		// Owner of this component is usually ProjectileActor
		// Instigator of ProjectileActor should be the Pawn who launch it
		BoomerangOwner = GetOwner()->GetInstigator();
	}
}

void UCustomProjectileComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bCancelHomingIfPassedTarget && IsHomingProjectile() && HasPassedHomingTarget())
	{
		ClearSocketHomingInfo();
	}

	if (IsBoomerang() && !bBoomerangReturning && Velocity.Size() <= BoomerangVelocityToReturn)
	{
		bBoomerangReturning = true;
		if (OnBoomerangStartReturning.IsBound())
		{
			OnBoomerangStartReturning.Execute();
		}
	}

	if (IsBoomerang() && bBoomerangReturning && GetDistanceToBoomerangOwner() <= OwnerDistanceToEndBoomerang)
	{
		StopSimulating(FHitResult());
	}
}

FVector UCustomProjectileComponent::ComputeVelocity(FVector InitialVelocity, float DeltaTime) const
{
	// v = v0 + a*t
	const FVector Acceleration = ComputeAcceleration(InitialVelocity, DeltaTime);
	FVector NewVelocity = InitialVelocity + (Acceleration * DeltaTime);

	if (bBoomerangReturning && BoomerangOwner.Get() && GetDistanceToBoomerangOwner() <= OwnerDistanceToMagnetizeBoomerang)
	{
		const FVector Dir = (BoomerangOwner->GetActorLocation() - UpdatedComponent->GetComponentLocation()).GetSafeNormal();
		NewVelocity = Dir * NewVelocity.Size();
	}

	return LimitVelocity(NewVelocity);
}

FVector UCustomProjectileComponent::ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const
{
	FVector Acceleration(FVector::ZeroVector);

	Acceleration.Z += GetGravityZ();

	Acceleration += PendingForceThisUpdate;

	if (IsHomingToTarget() || IsHomingToDestination())
	{
		Acceleration += ComputeHomingAcceleration(InVelocity, DeltaTime);
	}

	if (IsBoomerang())
	{
		Acceleration += ComputeBoomerangAcceleration(InVelocity, DeltaTime);
	}

	return Acceleration;
}

FVector UCustomProjectileComponent::ComputeHomingAcceleration(const FVector& InVelocity, float DeltaTime) const
{
	switch (HomingType)
	{
	case EHomingType::ToMovingTarget:
		{
			const FVector TargetLoc = HomingTargetComponent->GetSocketLocation(HomingSocketName);
	
			return bHoming2D ? (TargetLoc - UpdatedComponent->GetComponentLocation()).GetSafeNormal2D() * HomingAccelerationMagnitude :
				(TargetLoc - UpdatedComponent->GetComponentLocation()).GetSafeNormal() * HomingAccelerationMagnitude;
		}
	case EHomingType::ToDestination:
		{
			const FVector TargetLoc = HomingDestination.GetValue();
			
			return bHoming2D ? (TargetLoc - UpdatedComponent->GetComponentLocation()).GetSafeNormal2D() * HomingAccelerationMagnitude :
				(TargetLoc - UpdatedComponent->GetComponentLocation()).GetSafeNormal() * HomingAccelerationMagnitude;
		}
	default: ;
		return FVector(0);
	}
}

bool UCustomProjectileComponent::ShouldUseSubStepping() const
{
	return bForceSubStepping || GetGravityZ() != 0.0f || IsHomingToTarget() || IsHomingToDestination();
}

void UCustomProjectileComponent::StopSimulating(const FHitResult& HitResult)
{
	Super::StopSimulating(HitResult);

	if (OnBoomerangFinished.IsBound())
	{
		OnBoomerangFinished.Execute();
	}
}

FVector UCustomProjectileComponent::ComputeBoomerangAcceleration(const FVector& InVelocity, float DeltaTime) const
{
	if (bBoomerangReturning && BoomerangOwner.Get())
	{
		const FVector Dir = (BoomerangOwner->GetActorLocation() - UpdatedComponent->GetComponentLocation()).GetSafeNormal();
		return Dir * BoomerangReturningAcceleration;
	}

	// Decelerate
	return InVelocity.GetSafeNormal() * -FMath::Abs(BoomerangForwardDeceleration);
}

void UCustomProjectileComponent::RegisterSocketHoming(USceneComponent* HomingComp, const FName& BoneName)
{
	SetHomingType(EHomingType::ToMovingTarget);
	HomingTargetComponent = HomingComp;
	HomingSocketName = BoneName;

	//PLFL::PrintWarning(GetNameSafe(HomingTargetComponent.Get()) + " | " + HomingSocketName.ToString());
}

void UCustomProjectileComponent::RegisterSocketHomingFromActor(AActor* TargetActor, const bool bTargetWeakPoint)
{
	if (!TargetActor) return;
	
	if (UKismetSystemLibrary::DoesImplementInterface(TargetActor, UProjectileTargetInterface::StaticClass()))
	{
		USceneComponent* HomeComp = nullptr;
		if (bTargetWeakPoint)
		{
			IProjectileTargetInterface::Execute_GetWeakPointHomingInfo(TargetActor, HomeComp, HomingSocketName);
		}
		else
		{
			IProjectileTargetInterface::Execute_GetDefaultHomingInfo(TargetActor, HomeComp, HomingSocketName);
		}

		RegisterSocketHoming(HomeComp, HomingSocketName);
	}
	else
	{
		RegisterSocketHoming(TargetActor->GetRootComponent(), FName());
	}
}

void UCustomProjectileComponent::RegisterDestinationHoming(const FVector& Destination)
{
	SetHomingType(EHomingType::ToDestination);
	HomingDestination = Destination;
}

void UCustomProjectileComponent::ClearSocketHomingInfo()
{
	SetHomingType(EHomingType::NoHoming);
	HomingTargetComponent = nullptr;
	HomingSocketName = FName();
}

bool UCustomProjectileComponent::IsHomingProjectile() const
{
	return bIsHomingProjectile && HomingType != EHomingType::NoHoming;
}

bool UCustomProjectileComponent::IsHomingToTarget() const
{
	return HomingType == EHomingType::ToMovingTarget && HomingTargetComponent.Get();
}

bool UCustomProjectileComponent::IsHomingToDestination() const
{
	return HomingType == EHomingType::ToDestination && HomingDestination.IsSet();
}

void UCustomProjectileComponent::SetHomingType(const EHomingType InHomingType)
{
	HomingType = InHomingType;
	bIsHomingProjectile = (HomingType != EHomingType::NoHoming);
}

bool UCustomProjectileComponent::IsBoomerang() const
{
	return bIsBoomerang;
}

bool UCustomProjectileComponent::HasPassedHomingTarget() const
{
	if (!IsHomingProjectile()) return false;

	if (IsHomingToTarget())
	{
		return !HFL::IsTargetComponentInFront(GetOwner(), HomingTargetComponent.Get());
	}

	if (IsHomingToDestination())
	{
		return !HFL::IsTargetLocationInFront(GetOwner(), HomingDestination.GetValue());
	}

	return false;
}

float UCustomProjectileComponent::GetDistanceToBoomerangOwner() const
{
	check(GetOwner());
	return GetOwner()->GetDistanceTo(BoomerangOwner.Get());
}
