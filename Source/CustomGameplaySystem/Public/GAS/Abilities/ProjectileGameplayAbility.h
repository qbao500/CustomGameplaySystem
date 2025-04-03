// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageGameplayAbility.h"
#include "Actors/ProjectileActor.h"
#include "ProjectileGameplayAbility.generated.h"

class AProjectileActor;

/**
 * Avatar Actor needs to implement IProjectileShooterInterface to override spawn location, and maybe more as this gets updated.
 * Functions needed to override for this:
 *	- GetProjectileSpawnLocationForAbility()
 */
UCLASS(Abstract)
class CUSTOMGAMEPLAYSYSTEM_API UProjectileGameplayAbility : public UDamageGameplayAbility
{
	GENERATED_BODY()

protected:

	//~ Begin UGameplayAbility Interface
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	//~ End UGameplayAbility Interface
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (AutoCreateRefTerm = "OptionalHomingInfo"))
	void SpawnProjectileAndLaunch(const FVector& TargetLocation, const FProjectileHomingInfo& OptionalHomingInfo = FProjectileHomingInfo());
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int32 GetNumProjectiles() const;

	// Override (optional) this if bAutoLaunchProjectileWhenReceiveEvent is true. Otherwise it will launch forward.
	// This is called when receive LaunchEventToListen.
	UFUNCTION(BlueprintNativeEvent, Category = "Auto Launch")
	FVector GetTargetLocationForAutoLaunch() const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftClassPtr<AProjectileActor> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float LaunchForce = 1500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SpreadAngle = 25.0f;

	// Override (optional) GetTargetLocationForAutoLaunch() if this is true. Otherwise it will launch forward.
	// If false, you can manually launch projectile with SpawnProjectileAndLaunch().
	// You can override OnProjectileLaunched() to do something after projectile is launched.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Auto Launch")
	bool bAutoLaunchWhenReceiveEvent = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Auto Launch", meta = (Categories = "Event", EditCondition = "bAutoLaunchWhenReceiveEvent"))
	FGameplayTag LaunchEventToListen = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Auto Launch", meta = (EditCondition = "bAutoLaunchWhenReceiveEvent"))
	bool bAutoLaunchHoming = false;

	// If true, the ability will end after projectile is launched.
	// Otherwise, you have to manually end the ability.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bAutoEndAbilityWhenLaunched = false;

	virtual void OnProjectileLaunched(AProjectileActor* Projectile, const FProjectileLaunchInfo& LaunchInfo);
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnProjectileLaunched")
	void K2_OnProjectileLaunched(AProjectileActor* Projectile, const FProjectileLaunchInfo& LaunchInfo);

private:

	UPROPERTY()
	TSubclassOf<AProjectileActor> LoadedProjectileClass;
	void LoadProjectile();
	
	UFUNCTION()
	void OnReceiveAutoLaunchEvent(FGameplayEventData Payload);

public:

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
