// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/CustomProjectileComponent.h"
#include "GameFramework/Actor.h"
#include "GAS/Abilities/DamageGameplayAbility.h"
#include "ProjectileActor.generated.h"

class UFeedbackEffectDataAsset;
class USphereComponent;
class UCustomProjectileComponent;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FProjectileHomingInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	EHomingType HomingType = EHomingType::NoHoming;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> HomingTarget;
	UPROPERTY(BlueprintReadWrite, meta = (EditCondition = "HomingTarget != nullptr"))
	bool bHomingToWeakPoint = false;
	UPROPERTY(BlueprintReadWrite)
	FVector HomingDestination = FVector(0);

	bool IsHomingToTarget() const;
	bool IsHomingToDestination() const;
};

USTRUCT(BlueprintType)
struct FProjectileLaunchInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector StartLocation = FVector(0);
	UPROPERTY(BlueprintReadWrite)
	FVector Direction = FVector(0);

	// Launch force, limited by MaxSpeed of ProjectileMovementComponent
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Force = 1000;

	// Optional
	UPROPERTY(BlueprintReadWrite)
	FProjectileHomingInfo HomingInfo = FProjectileHomingInfo();
	
	bool IsHomingToTarget() const;
	bool IsHomingToDestination() const;
};

/**
 * 
 */
UCLASS(Abstract)
class CUSTOMGAMEPLAYSYSTEM_API AProjectileActor : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AProjectileActor();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_LaunchProjectile(const FProjectileLaunchInfo& LaunchInfo);

	UFUNCTION(BlueprintCallable)
	void OverrideDamageInfo(const FAbilityDamageInfo& NewInfo);

	UFUNCTION(BlueprintPure)
	const FProjectileLaunchInfo& GetCachedLaunchInfo() const;
	UFUNCTION(BlueprintPure)
	float GetProjectileDamage() const;
	UFUNCTION(BlueprintPure)
	bool IsFlying() const;
	UFUNCTION(BlueprintPure)
	bool IsBoomerang() const;

	/* Modify if should launch to Weak Spot of the target. Require AimingTarget to implement IProjectileTargetInterface */
	UFUNCTION(BlueprintCallable)
	static void ModifyLaunchInfoForAimingTarget(UPARAM(ref) FProjectileLaunchInfo& LaunchInfo, AActor* AimingTarget,
		const FVector& AimingDirection, const FVector& AimingFromLocation, const float MaxAllowedAngle = 6.5f);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USphereComponent> SphereCollider;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCustomProjectileComponent> ProjectileMovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = "OnRep_DamageInfo")
	FAbilityDamageInfo DamageInfo = FAbilityDamageInfo();
	UFUNCTION()
	void OnRep_DamageInfo(const FAbilityDamageInfo& OldInfo);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UFeedbackEffectDataAsset> ImpactEffectData;

	// If true, require shooter (owner) to implement IProjectileShooterInterface and override GetShooterColliders()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIgnoreCollisionWithShooter = true;

	// Setup if Projectile Component is a Boomerang, called in C++ BeginPlay()
	// By default, this function makes this projectile to overlap Pawn (instead of Block)
	// If override, MUST call parent function
	UFUNCTION(BlueprintNativeEvent)
	void SetupBoomerang();

	UFUNCTION(BlueprintNativeEvent)
	void LaunchProjectileLogic(const FProjectileLaunchInfo& LaunchInfo);

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Projectile Launched")
	void K2_OnProjectileLaunched(const FProjectileLaunchInfo& LaunchInfo);

	UFUNCTION(BlueprintCallable)
	void DealDamageAndDestroy(AActor* HitActor, const FHitResult& HitResult);
	UFUNCTION(BlueprintCallable)
	void DealDamageOnly(AActor* HitActor, const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable)
	void TriggerImpactEffect();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void OnBoomerangOverlapPawn(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

	void IgnoreOwnerCollision(const bool bShouldIgnore);

	void HandleDamage(AActor* HitActor, const FHitResult& HitResult);
	
	FProjectileLaunchInfo CachedLaunchInfo = FProjectileLaunchInfo();
	bool bClientAlreadyHit = false;
};
