// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ProjectileActor.h"

#include "Components/SphereComponent.h"
#include "DataAssets/FeedbackEffectDataAsset.h"
#include "FunctionLibraries/CustomHelperFunctionLibrary.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GAS/CustomAbilitySystemLibrary.h"
#include "GAS/Abilities/DamageGameplayAbility.h"
#include "Interfaces/ProjectileShooterInterface.h"
#include "Interfaces/ProjectileTargetInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ProjectileActor)

bool FProjectileHomingInfo::IsHomingToTarget() const
{
	return HomingType == EHomingType::ToMovingTarget && IsValid(HomingTarget);
}

bool FProjectileHomingInfo::IsHomingToDestination() const
{
	return HomingType == EHomingType::ToDestination;
}

bool FProjectileLaunchInfo::IsHomingToTarget() const
{
	return HomingInfo.IsHomingToTarget();
}

bool FProjectileLaunchInfo::IsHomingToDestination() const
{
	return HomingInfo.IsHomingToDestination();
}

AProjectileActor::AProjectileActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;

	SphereCollider = CreateDefaultSubobject<USphereComponent>("Sphere Collider");
	SetRootComponent(SphereCollider);
	SphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollider->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	SphereCollider->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	SphereCollider->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	ProjectileMovementComponent = CreateDefaultSubobject<UCustomProjectileComponent>("Projectile Movement Component");
}

void AProjectileActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjectileActor, DamageInfo);
}

void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();

	// Replicate movement
	SetReplicatingMovement(true);

	// Disable collision until projectile is launched
	SetActorEnableCollision(false);

	// Require shooter (owner) to implement IProjectileShooterInterface and override GetShooterColliders() 
	if (bIgnoreCollisionWithShooter)
	{
		IgnoreOwnerCollision(true);
	}
	
	// For damage
	SphereCollider->OnComponentHit.AddDynamic(this, &ThisClass::OnProjectileHit);

	// For boomerang
	if (IsBoomerang())
	{
		SetupBoomerang();
	}
}

void AProjectileActor::Destroyed()
{
	// If Server destroyed already hit and destroyed this projectile while Client still not, then trigger effect here for Client
	if (!bClientAlreadyHit && !HasAuthority())
	{
		TriggerImpactEffect();
	}
	
	Super::Destroyed();
}

void AProjectileActor::Server_LaunchProjectile_Implementation(const FProjectileLaunchInfo& LaunchInfo)
{
	CachedLaunchInfo = LaunchInfo;

	if (!HasAuthority()) return;
	
	LaunchProjectileLogic(LaunchInfo);
}

void AProjectileActor::OverrideDamageInfo(const FAbilityDamageInfo& NewInfo)
{
	DamageInfo = NewInfo;
}

const FProjectileLaunchInfo& AProjectileActor::GetCachedLaunchInfo() const
{
	return CachedLaunchInfo;
}

float AProjectileActor::GetProjectileDamage() const
{
	return DamageInfo.GetCurrentDamage();
}

bool AProjectileActor::IsFlying() const
{
	return ProjectileMovementComponent->Velocity.Size() > 50.0f;
}

bool AProjectileActor::IsBoomerang() const
{
	return ProjectileMovementComponent->IsBoomerang();
}

void AProjectileActor::ModifyLaunchInfoForAimingTarget(FProjectileLaunchInfo& LaunchInfo, AActor* AimingTarget,
	const FVector& AimingDirection, const FVector& AimingFromLocation, const float MaxAllowedAngle)
{
	if (!UKismetSystemLibrary::DoesImplementInterface(AimingTarget, UProjectileTargetInterface::StaticClass())) return;

	USceneComponent* HomingComp = nullptr;
	FName HomingSocket = FName();
	IProjectileTargetInterface::Execute_GetWeakPointHomingInfo(AimingTarget, HomingComp, HomingSocket);

	if (!HomingComp) return;

	const FVector WeakPointLoc = HomingComp->GetSocketLocation(HomingSocket);
	bool bShouldAimWeakSpot = false;
	const FVector BestAimLoc = HFL::ChooseBestAimingLocation(AimingDirection, AimingFromLocation,
		WeakPointLoc, AimingTarget->GetActorLocation(), bShouldAimWeakSpot);
	const FVector DirToTarget = (BestAimLoc - AimingFromLocation).GetSafeNormal();

	if (HFL::GetAngleBetweenDirections(DirToTarget, AimingDirection) > MaxAllowedAngle) return;
	
	LaunchInfo.Direction = FMath::Lerp(AimingDirection, DirToTarget, 0.8f);
	LaunchInfo.HomingInfo.HomingTarget = AimingTarget;
	LaunchInfo.HomingInfo.bHomingToWeakPoint = bShouldAimWeakSpot;
}

void AProjectileActor::OnRep_DamageInfo(const FAbilityDamageInfo& OldInfo)
{
	// Nothing for now
}

void AProjectileActor::LaunchProjectileLogic_Implementation(const FProjectileLaunchInfo& LaunchInfo)
{
	ProjectileMovementComponent->Activate();
	ProjectileMovementComponent->Velocity = LaunchInfo.Direction * LaunchInfo.Force;

	if (LaunchInfo.HomingInfo.HomingType == EHomingType::NoHoming)
	{
		ProjectileMovementComponent->SetHomingType(EHomingType::NoHoming);
	}
	else if (LaunchInfo.IsHomingToTarget())
	{
		ProjectileMovementComponent->RegisterSocketHomingFromActor(LaunchInfo.HomingInfo.HomingTarget, LaunchInfo.HomingInfo.bHomingToWeakPoint);
	}
	else if (LaunchInfo.IsHomingToDestination())
	{
		ProjectileMovementComponent->RegisterDestinationHoming(LaunchInfo.HomingInfo.HomingDestination);
	}

	// Enable collision after projectile is launched
	SetActorEnableCollision(true);

	// Event for BP
	K2_OnProjectileLaunched(LaunchInfo);
}

void AProjectileActor::SetupBoomerang_Implementation()
{
	SphereCollider->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ProjectileMovementComponent->OnBoomerangFinished.BindWeakLambda(this,
			[this]()
			{
				TriggerImpactEffect();
				Destroy();
			});

	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBoomerangOverlapPawn);
}

void AProjectileActor::DealDamageAndDestroy(AActor* HitActor, const FHitResult& HitResult)
{
	DealDamageOnly(HitActor, HitResult);
	Destroy();
}

void AProjectileActor::DealDamageOnly(AActor* HitActor, const FHitResult& HitResult)
{
	HandleDamage(HitActor, HitResult);
	TriggerImpactEffect();
}

void AProjectileActor::TriggerImpactEffect()
{
	if (!ImpactEffectData) return;

	ImpactEffectData->PlayDefaultEffects(this, GetActorLocation(), GetActorRotation());
}

void AProjectileActor::OnProjectileHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (HasAuthority())
	{
		if (IsBoomerang())
		{
			DealDamageOnly(OtherActor, Hit);
		}
		else
		{
			DealDamageAndDestroy(OtherActor, Hit);
		}
	}
	else
	{
		if (!IsBoomerang())
		{
			bClientAlreadyHit = true;
		}
	}
}

void AProjectileActor::OnBoomerangOverlapPawn(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Re-route to Hit event to damage pawn
	OnProjectileHit(OverlappedComponent, OtherActor, OtherComp, FVector(0), SweepResult);
}

void AProjectileActor::IgnoreOwnerCollision(const bool bShouldIgnore)
{
	if (UKismetSystemLibrary::DoesImplementInterface(GetOwner(), UProjectileShooterInterface::StaticClass()))
	{
		for (UPrimitiveComponent* OwnerCollider : IProjectileShooterInterface::Execute_GetShooterColliders(GetOwner()))
		{
			if (!OwnerCollider) return;
		
			SphereCollider->IgnoreActorWhenMoving(GetOwner(), bShouldIgnore);
			OwnerCollider->IgnoreActorWhenMoving(this, bShouldIgnore);
		}
	}
}

void AProjectileActor::HandleDamage(AActor* HitActor, const FHitResult& HitResult)
{
	if (DamageInfo.bRadialDamage)
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Emplace(this);
		IgnoredActors.Emplace(GetOwner());
		AFL::AbilityApplyRadialDamage(DamageInfo.GetAbility(), DamageInfo, GetActorLocation(), IgnoredActors);
	}
	else
	{
		AFL::AbilityApplyDamage(DamageInfo.GetAbility(), DamageInfo, HitActor, HitResult);
	}
}
