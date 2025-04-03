// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/CustomGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "GAS/CustomAbilitySystemGlobals.h"

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
