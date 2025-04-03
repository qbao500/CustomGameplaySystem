// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/ProjectileGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Actors/ProjectileActor.h"
#include "FunctionLibraries/CustomHelperFunctionLibrary.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "Interfaces/CombatInterface.h"
#include "Interfaces/ProjectileShooterInterface.h"
#include "Kismet/KismetSystemLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ProjectileGameplayAbility)

void UProjectileGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	// Only load this projectile when the ability is given.
	// Otherwise, if projectile is a hard ref, and if this ability is being referenced without actually needing it, then it will also auto-load projectile mesh and effect assets.
	LoadProjectile();
}

bool UProjectileGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!LoadedProjectileClass) return false;
	
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UProjectileGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (bAutoLaunchWhenReceiveEvent && LaunchEventToListen.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, LaunchEventToListen);
		Task->EventReceived.AddDynamic(this, &ThisClass::OnReceiveAutoLaunchEvent);
		Task->ReadyForActivation();
	}
}

int32 UProjectileGameplayAbility::GetNumProjectiles_Implementation() const
{
	return 1;
}

void UProjectileGameplayAbility::SpawnProjectileAndLaunch_Implementation(const FVector& TargetLocation, const FProjectileHomingInfo& OptionalHomingInfo)
{
	if (!LoadedProjectileClass) return;

	// Only spawn on server
	const bool bServer = HasAuthority(&GetCurrentActivationInfoRef());
	if (!bServer) return;

	AActor* AvatarActor = GetAvatarActorFromActorInfo();

	// Required to implement IProjectileShooterInterface for Avatar Actor.
	if (!UKismetSystemLibrary::DoesImplementInterface(AvatarActor, UProjectileShooterInterface::StaticClass())) return;
	
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	
	// Spawn transform
	const FVector StartLocation = IProjectileShooterInterface::Execute_GetProjectileSpawnLocationForAbility(AvatarActor, AbilityTags, ActivationOwnedTags);
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(StartLocation);

	// Fulfill the launch info
	FProjectileLaunchInfo LaunchInfo;
	LaunchInfo.StartLocation = StartLocation;
	LaunchInfo.Force = LaunchForce;
	LaunchInfo.HomingInfo = OptionalHomingInfo;

	// Effect context. AProjectileActor will add HitResult to DamageContextHandle, when it hits something.
	FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(this);
	ContextHandle.AddOrigin(StartLocation);

	TArray<TWeakObjectPtr<AActor>> Actors;

	const FVector TargetDirection = HFL::GetUnitDirection2D(StartLocation, TargetLocation);
	for (const FRotator& Rot : HFL::MakeSpreadRotators(GetNumProjectiles(), SpreadAngle, TargetDirection))
	{
		SpawnTransform.SetRotation(Rot.Quaternion());

		// Spawn deferred
		AProjectileActor* Projectile = GetWorld()->SpawnActorDeferred<AProjectileActor>(LoadedProjectileClass, SpawnTransform,
			AvatarActor, Cast<APawn>(AvatarActor), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		check(Projectile);

		// Add projectile to context
		Actors.Reset();
		Actors.Emplace(Projectile);
		ContextHandle.AddActors(Actors, true);
		DamageInfo.DamageContextHandle = ContextHandle;
		
		// Assign damage to the effect spec
		const FGameplayEffectSpecHandle& SpecHandle = SourceASC->MakeOutgoingSpec(DamageInfo.DamageGE, GetAbilityLevel(), ContextHandle);
		AssignDamageToEffectSpec(SpecHandle);
		
		// Override DamageInfo
		Projectile->OverrideDamageInfo(DamageInfo);
		
		// Assign the rest of launch info
		LaunchInfo.Direction = Rot.Vector();

		// Full spawn
		Projectile->FinishSpawning(SpawnTransform);

		// Now finally launch the projectile
		Projectile->Server_LaunchProjectile(LaunchInfo);

		// Events
		OnProjectileLaunched(Projectile, LaunchInfo);
		K2_OnProjectileLaunched(Projectile, LaunchInfo);
	}
}

FVector UProjectileGameplayAbility::GetTargetLocationForAutoLaunch_Implementation() const
{
	// Default to launch forward
	return GetAvatarLocation() + GetAvatarForwardVector() * 100;
}

void UProjectileGameplayAbility::OnProjectileLaunched(AProjectileActor* Projectile,	const FProjectileLaunchInfo& LaunchInfo)
{
	if (bAutoEndAbilityWhenLaunched)
	{
		K2_EndAbility();
	}
}

void UProjectileGameplayAbility::LoadProjectile()
{
	ensureAlways(!ProjectileClass.IsNull());
	
	HFL::AsyncLoad(ProjectileClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateWeakLambda(this, [this]
		{
			LoadedProjectileClass = ProjectileClass.Get();
		}));
}

void UProjectileGameplayAbility::OnReceiveAutoLaunchEvent(FGameplayEventData Payload)
{
	const FVector& TargetLocation = GetTargetLocationForAutoLaunch();
	FProjectileHomingInfo HomingInfo;

	if (bAutoLaunchHoming)
	{
		AActor* HomingTarget = nullptr;
		if (UKismetSystemLibrary::DoesImplementInterface(GetAvatarActorFromActorInfo(), UCombatInterface::StaticClass()))
		{
			HomingTarget = ICombatInterface::Execute_GetCombatTarget(GetAvatarActorFromActorInfo());
		}

		if (HomingTarget)
		{
			HomingInfo.HomingType = EHomingType::ToMovingTarget;
			HomingInfo.HomingTarget = HomingTarget;
		}
		else
		{
			HomingInfo.HomingType = EHomingType::ToDestination;
			HomingInfo.HomingDestination = TargetLocation;
		}
	}
	
	SpawnProjectileAndLaunch(TargetLocation, HomingInfo);
}

#if WITH_EDITOR
void UProjectileGameplayAbility::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

}
#endif