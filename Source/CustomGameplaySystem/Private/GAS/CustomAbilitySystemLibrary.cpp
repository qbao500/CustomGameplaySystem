// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CustomAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CustomGameplayTags.h"
#include "CustomLogChannels.h"
#include "GameplayEffect.h"
#include "DataAssets/CustomAbilityInfoDataAsset.h"
#include "Engine/DamageEvents.h"
#include "FunctionLibraries/CustomHelperFunctionLibrary.h"
#include "FunctionLibraries/GameplayTagFunctionLibrary.h"
#include "GAS/Abilities/DamageGameplayAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomAbilitySystemLibrary)

void UCustomAbilitySystemLibrary::ApplyEffectSetByCaller(UAbilitySystemComponent* ASC,
	const TSubclassOf<UGameplayEffect>& Effect, const FGameplayTag& DataTag, const float Magnitude,
	UObject* OptionalSourceObject)
{
	if (!ASC || !Effect || !DataTag.IsValid()) return;
	
	const FGameplayEffectSpecHandle& SpecHandle = ASC->MakeOutgoingSpec(Effect, 1, MakeEffectContextWithObject(ASC, OptionalSourceObject));
	
	if (FGameplayEffectSpec* Spec = SpecHandle.Data.Get())
	{
		Spec->SetSetByCallerMagnitude(DataTag, Magnitude);
		ASC->ApplyGameplayEffectSpecToSelf(*Spec);
	}
}

void UCustomAbilitySystemLibrary::ApplyEffectSetByCaller(UAbilitySystemComponent* ASC,
	const TSubclassOf<UGameplayEffect>& Effect, const FName& DataTagName, const float Magnitude,
	UObject* OptionalSourceObject)
{
	ApplyEffectSetByCaller(ASC, Effect, TAG_FL::MakeTag(DataTagName), Magnitude, OptionalSourceObject);
}

TArray<FGameplayTag> UCustomAbilitySystemLibrary::GetSetByCallerTags(const TSubclassOf<UGameplayEffect>& FromGameplayEffect)
{
	TArray<FGameplayTag> Tags;

	if (!FromGameplayEffect) return Tags;

	UGameplayEffect* CDO = FromGameplayEffect->GetDefaultObject<UGameplayEffect>();
	if (!CDO) return Tags;

	for (const FGameplayModifierInfo& ModifierInfo : CDO->Modifiers)
	{
		if (ModifierInfo.ModifierMagnitude.GetMagnitudeCalculationType() == EGameplayEffectMagnitudeCalculation::SetByCaller)
		{
			Tags.Emplace(ModifierInfo.ModifierMagnitude.GetSetByCallerFloat().DataTag);
		}
	}

	return Tags;
}

FGameplayEffectContextHandle UCustomAbilitySystemLibrary::MakeEffectContextWithObject(const UAbilitySystemComponent* ASC,
	const UObject* SourceObject)
{
	if (!ASC) return FGameplayEffectContextHandle();

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	if (SourceObject) Context.AddSourceObject(SourceObject);

	return Context;
}

FGameplayAbilitySpec UCustomAbilitySystemLibrary::GetAbilitySpecFromTag(UAbilitySystemComponent* ASC,
	const FGameplayTag& AbilityTag, bool& bFound)
{
	bFound = false;

	if (!ASC) return FGameplayAbilitySpec();

	// Lock the scope of this function, so we won't run into complications from abilities being added/removed while we're iterating
	FScopedAbilityListLock ActiveScopeLock(*ASC);

	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (IsAbilityTagInSpec(Spec, AbilityTag))
		{
			bFound = true;
			return Spec;
		}
	}

	return FGameplayAbilitySpec();
}

FGameplayAbilitySpec* UCustomAbilitySystemLibrary::GetAbilitySpecFromTag(UAbilitySystemComponent* ASC,
	const FGameplayTag& AbilityTag)
{
	if (!ASC) return nullptr;

	// Lock the scope of this function, so we won't run into complications from abilities being added/removed while we're iterating
	FScopedAbilityListLock ActiveScopeLock(*ASC);

	for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (IsAbilityTagInSpec(Spec, AbilityTag))
		{
			return &Spec;
		}
	}

	return nullptr;
}

UGameplayAbility* UCustomAbilitySystemLibrary::GetAbilityInstanceFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (UGameplayAbility* PrimaryInstance = AbilitySpec.GetPrimaryInstance())
	{
		return PrimaryInstance;
	}

	return AbilitySpec.Ability;
}

const FGameplayTagContainer& UCustomAbilitySystemLibrary::GetAbilityTagsFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (UGameplayAbility* Instance = GetAbilityInstanceFromSpec(AbilitySpec))
	{
		return Instance->AbilityTags;
	}

	return FGameplayTagContainer::EmptyContainer;
}

const FGameplayTag& UCustomAbilitySystemLibrary::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (const FGameplayTag& Tag : AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(CustomTags::Input))
		{
			return Tag;
		}
	}

	return FGameplayTag::EmptyTag;
}

bool UCustomAbilitySystemLibrary::IsAbilityTagInSpec(const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& AbilityTag)
{
	const FGameplayTagContainer& AbilityTags = GetAbilityTagsFromSpec(AbilitySpec);
	return AbilityTags.HasTag(AbilityTag);
}

const FGameplayTag& UCustomAbilitySystemLibrary::GetFirstAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (const UGameplayAbility* Instance = GetAbilityInstanceFromSpec(AbilitySpec))
	{
		return TAG_FL::GetFirstGameplayTagFromContainer(Instance->AbilityTags);
	}

	return FGameplayTag::EmptyTag;
}

const FGameplayTag& UCustomAbilitySystemLibrary::GetFirstAbilityTagFromAbility(const TSubclassOf<UGameplayAbility>& Ability)
{
	if (!Ability) return FGameplayTag::EmptyTag;

	if (const UGameplayAbility* CDO = Ability->GetDefaultObject<UGameplayAbility>())
	{
		return TAG_FL::GetFirstGameplayTagFromContainer(CDO->AbilityTags);
	}

	return FGameplayTag::EmptyTag;
}

const FGameplayTagContainer& UCustomAbilitySystemLibrary::GetDynamicAbilityTagsFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	return AbilitySpec.DynamicAbilityTags;
}

bool UCustomAbilitySystemLibrary::DoesAbilityHaveInputTag(const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& InputTag)
{
	return AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag);
}

bool UCustomAbilitySystemLibrary::DoesAbilityHaveInputTag(const FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& InputTag)
{
	if (!AbilitySpec) return false;
	
	return AbilitySpec->DynamicAbilityTags.HasTagExact(InputTag);
}

void UCustomAbilitySystemLibrary::AbilityApplyDamage(const UGameplayAbility* Ability, const FAbilityDamageInfo& AbilityDamageInfo,
	AActor* TargetActor, const FHitResult& HitResult)
{
	if (!TargetActor) return;

	UAbilitySystemComponent* SourceASC = AbilityDamageInfo.GetAbilitySystemComponent();
	check(SourceASC);
	
	AActor* AvatarActor = SourceASC->GetAvatarActor();
	const float Damage = AbilityDamageInfo.GetCurrentDamage();

	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
	{
		FGameplayEffectContextHandle ContextHandle = AbilityDamageInfo.DamageContextHandle;
		if (!ContextHandle.IsValid())
		{
			ContextHandle = SourceASC->MakeEffectContext();
			ContextHandle.SetAbility(Ability);
			ContextHandle.AddSourceObject(Ability);
		}
		
		ContextHandle.AddHitResult(HitResult, true);
		
		const FGameplayEffectSpecHandle& SpecHandle = SourceASC->MakeOutgoingSpec(AbilityDamageInfo.DamageGE, AbilityDamageInfo.GetAbilityLevel(), ContextHandle);
		if (FGameplayEffectSpec* Spec = SpecHandle.Data.Get())
		{
			Spec->SetSetByCallerMagnitude(AbilityDamageInfo.DamageTag, Damage);
		}

		SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	}
	else
	{
		// Normal apply damage
		const FVector ShotDir = (TargetActor->GetActorLocation() - AvatarActor->GetActorLocation()).GetSafeNormal();
		FPointDamageEvent PointDamageEvent (Damage, HitResult, ShotDir, nullptr);
		TargetActor->TakeDamage(Damage, PointDamageEvent, AvatarActor->GetInstigatorController(), AvatarActor);
	}
}

bool UCustomAbilitySystemLibrary::AbilityApplyRadialDamage(const UGameplayAbility* Ability, const FAbilityDamageInfo& AbilityDamageInfo,
                                                           const FVector& Origin, const TArray<AActor*>& IgnoredActors)
{
	UAbilitySystemComponent* SourceASC = AbilityDamageInfo.GetAbilitySystemComponent();
	check(SourceASC);

	if (!AbilityDamageInfo.bRadialDamage)
	{
		return false;
	}
	
	AActor* DamageCauser = SourceASC->GetAvatarActor();
	check(DamageCauser);
	
	FCollisionQueryParams SphereParams(SCENE_QUERY_STAT(GA_ApplyRadialDamage),  false, DamageCauser);
	SphereParams.AddIgnoredActors(IgnoredActors);

	// Query scene to see what we hit
	TArray<FOverlapResult> Overlaps;
	if (const UWorld* World = DamageCauser->GetWorld())
	{
		World->OverlapMultiByObjectType(Overlaps, Origin, FQuat::Identity,
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(AbilityDamageInfo.RadialOuterRadius), SphereParams);
	}

	// Collate into per-actor list of hit components
	TMap<AActor*, TArray<FHitResult> > OverlapComponentMap;
	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* const OverlapActor = Overlap.OverlapObjectHandle.FetchActor();

		if (OverlapActor &&
			OverlapActor->CanBeDamaged() &&
			OverlapActor != DamageCauser &&
			Overlap.Component.IsValid())
		{
			FHitResult Hit;
			if (HFL::ComponentIsDamageableFrom(!AbilityDamageInfo.bDoTracePreventRadialDamage, Overlap.Component.Get(),
				Origin, DamageCauser, IgnoredActors, AbilityDamageInfo.RadialDamagePreventionChannel, Hit))
			{
				TArray<FHitResult>& HitList = OverlapComponentMap.FindOrAdd(OverlapActor);
				HitList.Add(Hit);
			}
		}
	}

	if (OverlapComponentMap.Num() <= 0) return false;

	const float BaseDamage = AbilityDamageInfo.GetCurrentDamage();
	FRadialDamageEvent DamageEvent;
	DamageEvent.Origin = Origin;
	DamageEvent.Params = FRadialDamageParams(BaseDamage, AbilityDamageInfo.RadialMinimumDamage, AbilityDamageInfo.RadialInnerRadius,
		AbilityDamageInfo.RadialOuterRadius, AbilityDamageInfo.RadialDamageFalloff);
	
	FGameplayEffectContextHandle ContextHandle = AbilityDamageInfo.DamageContextHandle;
	if (!ContextHandle.IsValid())
	{
		ContextHandle = SourceASC->MakeEffectContext();
		ContextHandle.SetAbility(Ability);
		ContextHandle.AddSourceObject(Ability);
	}

	// Call damage function on each affected actors
	for (TMap<AActor*, TArray<FHitResult>>::TIterator It(OverlapComponentMap); It; ++It)
	{
		AActor* Target = It.Key();
		if (!Target) continue;
		
		const TArray<FHitResult>& ComponentHits = It.Value();
		DamageEvent.ComponentHits = ComponentHits;

		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
		{
			FHitResult HitResult;
			const float FinalDamage = HFL::CalculateRadialDamage(DamageEvent, HitResult);
			
			ContextHandle.AddHitResult(HitResult, true);
			
			const FGameplayEffectSpecHandle& SpecHandle = SourceASC->MakeOutgoingSpec(AbilityDamageInfo.DamageGE, AbilityDamageInfo.GetAbilityLevel(), ContextHandle);
			if (FGameplayEffectSpec* Spec = SpecHandle.Data.Get())
			{
				Spec->SetSetByCallerMagnitude(AbilityDamageInfo.DamageTag, FinalDamage);
			}
			
			SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
		}
		else
		{
			// Use default TakeDamage with damage event of RadialDamage
			Target->TakeDamage(BaseDamage, DamageEvent, DamageCauser->GetInstigatorController(), DamageCauser);
		}

		// Apply physics impulse
		for (const FHitResult& Hit : ComponentHits)
		{
			UPrimitiveComponent* PrimitiveComponent = Hit.GetComponent();
			if (PrimitiveComponent && PrimitiveComponent->GetOwner() == Target && PrimitiveComponent->IsSimulatingPhysics())
			{
				// Cheating a bit with radius, so it's easier to apply the impulse
				PrimitiveComponent->AddRadialImpulse(Origin, AbilityDamageInfo.RadialOuterRadius * 1.5f,
					AbilityDamageInfo.PhysicsRadialImpulseStrength, RIF_Linear, true);
			}
		}
	}
	
	return true;
}

FPrimaryAssetId UCustomAbilitySystemLibrary::MakeAbilityAssetId(const FGameplayTag& AbilityTag)
{
	return FPrimaryAssetId(ABILITY_DATA, AbilityTag.GetTagName());
}

FGameplayAbilitySpec* UCustomAbilitySystemLibrary::GetAbilitySpecFromInput(UAbilitySystemComponent* ASC,
	const FGameplayTag& InputTag)
{
	if (!ASC) return nullptr;

	FScopedAbilityListLock ActiveScopeLock(*ASC);

	for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (DoesAbilityHaveInputTag(Spec, InputTag))
		{
			return &Spec;
		}
	}

	return nullptr;
}

FAggregatorEvaluateParameters UCustomAbilitySystemLibrary::GetAggregatorEvaluateParamsByEffectSpec(const FGameplayEffectSpec& Spec)
{
	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	return EvaluateParameters;
}