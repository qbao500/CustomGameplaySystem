// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/CustomGameplayAbility.h"

#include "CustomGameplayTags.h"
#include "GAS/CustomAbilitySystemGlobals.h"
#include "GAS/Components/CustomAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomGameplayAbility)

UCustomGameplayAbility::UCustomGameplayAbility()
{
	// This is already the default value of UE's Gameplay Ability, but still write here for show the value.
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// We want most of the abilities to be given once and stick around.
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// Abilities are usually Active, by input. For passive abilities, we may set this to OnSpawn.
	ActivationPolicy = EAbilityActivationPolicy::OnInputTriggered;
}

void UCustomGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	K2_OnGiveAbility(Spec);

	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

bool UCustomGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	// Specialized version to handle death exclusion and AbilityTags expansion via ASC

	bool bBlocked = false;
	bool bMissing = false;

	const UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
	const FGameplayTag& BlockedTag = AbilitySystemGlobals.ActivateFailTagsBlockedTag;
	const FGameplayTag& MissingTag = AbilitySystemGlobals.ActivateFailTagsMissingTag;

	// Check if any of this ability's tags are currently blocked
	if (AbilitySystemComponent.AreAbilityTagsBlocked(AbilityTags))
	{
		bBlocked = true;
	}

	const UCustomAbilitySystemComponent* CustomASC = Cast<UCustomAbilitySystemComponent>(&AbilitySystemComponent);
	static FGameplayTagContainer AllRequiredTags;
	static FGameplayTagContainer AllBlockedTags;

	AllRequiredTags = ActivationRequiredTags;
	AllBlockedTags = ActivationBlockedTags;

	// Expand our ability tags to add additional required/blocked tags
	if (CustomASC)
	{
		CustomASC->GetAdditionalActivationTagRequirements(AbilityTags, AllRequiredTags, AllBlockedTags);
	}

	// Check to see the required/blocked tags for this ability
	if (AllBlockedTags.Num() || AllRequiredTags.Num())
	{
		static FGameplayTagContainer AbilitySystemComponentTags;
		
		AbilitySystemComponentTags.Reset();
		AbilitySystemComponent.GetOwnedGameplayTags(AbilitySystemComponentTags);

		if (AbilitySystemComponentTags.HasAny(AllBlockedTags))
		{
			if (OptionalRelevantTags && AbilitySystemComponentTags.HasTag(CustomTags::Status_Death))
			{
				// If player is dead and was rejected due to blocking tags, give that feedback
				OptionalRelevantTags->AddTag(CustomTags::Ability_ActivateFail_IsDead);
			}

			bBlocked = true;
		}

		if (!AbilitySystemComponentTags.HasAll(AllRequiredTags))
		{
			bMissing = true;
		}
	}

	if (SourceTags != nullptr)
	{
		if (SourceBlockedTags.Num() || SourceRequiredTags.Num())
		{
			if (SourceTags->HasAny(SourceBlockedTags))
			{
				bBlocked = true;
			}

			if (!SourceTags->HasAll(SourceRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	if (TargetTags != nullptr)
	{
		if (TargetBlockedTags.Num() || TargetRequiredTags.Num())
		{
			if (TargetTags->HasAny(TargetBlockedTags))
			{
				bBlocked = true;
			}

			if (!TargetTags->HasAll(TargetRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	if (bBlocked)
	{
		if (OptionalRelevantTags && BlockedTag.IsValid())
		{
			OptionalRelevantTags->AddTag(BlockedTag);
		}
		return false;
	}
	if (bMissing)
	{
		if (OptionalRelevantTags && MissingTag.IsValid())
		{
			OptionalRelevantTags->AddTag(MissingTag);
		}
		return false;
	}

	return true;
}

void UCustomGameplayAbility::InputPressed(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	
	K2_InputPressed(Handle, ActivationInfo);
}

void UCustomGameplayAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	K2_InputReleased(Handle, ActivationInfo);
}

float UCustomGameplayAbility::GetAbilityCost(const int32 Level, const FGameplayAttribute& AttributeToCheck) const
{
	const UGameplayEffect* CostEffect = GetCostGameplayEffect();
	if (!CostEffect) return 0.0f;

	const bool bAll = !AttributeToCheck.IsValid();

	float TotalCost = 0.0f;
	for (const FGameplayModifierInfo& Mod : CostEffect->Modifiers)
	{
		if (bAll || Mod.Attribute == AttributeToCheck)
		{
			float Cost = 0.0f;
			Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(Level, Cost);
			Cost = FMath::Abs(Cost);
			TotalCost += Cost;

			// If we're not checking all, then we can break early
			if (!bAll) break;
		}
	}

	return TotalCost;
}

float UCustomGameplayAbility::GetAbilityCooldown(const int32 Level) const
{
	const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect();
	if (!CooldownEffect) return 0.0f;

	float Cooldown = 0.0f;
	CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(Level, Cooldown);

	return FMath::Abs(Cooldown);
}

bool UCustomGameplayAbility::DoesEffectContextHaveThisAbility(const FGameplayEffectContextHandle& EffectContext) const
{
	const UGameplayAbility* AbilityCDO = EffectContext.GetAbility();
	if (!AbilityCDO) return false;

	if (AbilityCDO->StaticClass() == this->StaticClass())
	{
		return true;
	}

	// Fallback to Source Object
	const UObject* SourceObj = EffectContext.GetSourceObject();
	if (SourceObj && SourceObj == this)
	{
		return true;
	}

	return false;
}

void UCustomGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	// TODO Reconsider
	// No need to call CommitAbility in BP anymore
	/*if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{			
		constexpr bool bReplicateEndAbility = true;
		constexpr bool bWasCancelled = true;
		EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
		return;
	}*/
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UCustomGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
	const bool bIsPredicting = (Spec.ActivationInfo.ActivationMode == EGameplayAbilityActivationMode::Predicting);

	// Try to activate if activation policy is on spawn.
	if (ActorInfo && !Spec.IsActive() && !bIsPredicting && (ActivationPolicy == EAbilityActivationPolicy::OnSpawn))
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		// If avatar actor is torn off or about to die, don't try to activate until we get the new one.
		if (ASC && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.0f))
		{
			const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
			const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);

			const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
			const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

			if (bClientShouldActivate || bServerShouldActivate)
			{
				ASC->TryActivateAbility(Spec.Handle);
			}
		}
	}
}

void UCustomGameplayAbility::OnPawnAvatarSet(APawn* NewAvatar)
{
	K2_OnPawnAvatarSet(NewAvatar);
}

FText UCustomGameplayAbility::FillAbilityDescriptionWithFormat_Implementation(const FText& Description, int32 Level)
{
	FFormatNamedArguments Args;
	Args.Add(CostKeyToFormat, GetAbilityCost(Level));
	Args.Add(CooldownKeyToFormat, GetAbilityCooldown(Level));
	
	return Description;
}

FVector UCustomGameplayAbility::GetAvatarLocation() const
{
	return GetAvatarActorFromActorInfo()->GetActorLocation();
}

FVector UCustomGameplayAbility::GetAvatarForwardVector() const
{
	return GetAvatarActorFromActorInfo()->GetActorForwardVector();
}

FVector UCustomGameplayAbility::GetAvatarRightVector() const
{
	return GetAvatarActorFromActorInfo()->GetActorRightVector();
}

FVector UCustomGameplayAbility::GetAvatarUpVector() const
{
	return GetAvatarActorFromActorInfo()->GetActorUpVector();
}

FRotator UCustomGameplayAbility::GetAvatarRotation() const
{
	return GetAvatarActorFromActorInfo()->GetActorRotation();
}

FQuat UCustomGameplayAbility::GetAvatarQuat() const
{
	return GetAvatarActorFromActorInfo()->GetActorQuat();
}

FVector UCustomGameplayAbility::GetAvatarScale() const
{
	return GetAvatarActorFromActorInfo()->GetActorScale3D();
}

const FTransform& UCustomGameplayAbility::GetAvatarTransform() const
{
	return GetAvatarActorFromActorInfo()->GetActorTransform();
}

USceneComponent* UCustomGameplayAbility::GetAvatarRootComponent() const
{
	return GetAvatarActorFromActorInfo()->GetRootComponent();
}

const FGameplayTagContainer& UCustomGameplayAbility::GetDamageTags()
{
	return UCustomAbilitySystemGlobals::GetDamageTags();
}

FTimerManager& UCustomGameplayAbility::GetTimerManager() const
{
	return GetWorld()->GetTimerManager();
}
