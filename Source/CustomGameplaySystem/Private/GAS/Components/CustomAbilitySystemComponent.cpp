// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Components/CustomAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "CustomGameplayTags.h"
#include "CustomLogChannels.h"
#include "DataAssets/CustomAbilityInfoDataAsset.h"
#include "Engine/AssetManager.h"
#include "GAS/CustomAbilitySystemLibrary.h"
#include "FunctionLibraries/CustomHelperFunctionLibrary.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "GAS/CustomAbilitySystemGlobals.h"
#include "GAS/Abilities/CustomGameplayAbility.h"
#include "GAS/CustomGlobalAbilitySubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomAbilitySystemComponent)

void UCustomAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCustomAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UCustomGlobalAbilitySubsystem* GlobalAbilitySystem = UWorld::GetSubsystem<UCustomGlobalAbilitySubsystem>(GetWorld()))
	{
		GlobalAbilitySystem->UnregisterASC(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

void UCustomAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	const FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);
	
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
	
	APawn* AvatarPawn = Cast<APawn>(InAvatarActor);
	const bool bHasNewPawnAvatar = AvatarPawn && (InAvatarActor != ActorInfo->AvatarActor);
	if (!bHasNewPawnAvatar) return;

	// Notify all abilities that a new pawn avatar has been set
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		UCustomGameplayAbility* AbilityCDO = Cast<UCustomGameplayAbility>(AbilitySpec.Ability);
		if (!AbilityCDO)
		{
			continue;
		}

		if (AbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
		{
			TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
			for (UGameplayAbility* AbilityInstance : Instances)
			{
				if (UCustomGameplayAbility* CustomAbilityInstance = Cast<UCustomGameplayAbility>(AbilityInstance))
				{
					// Ability instances may be missing for replays
					CustomAbilityInstance->OnPawnAvatarSet(AvatarPawn);
				}
			}
		}
		else
		{
			AbilityCDO->OnPawnAvatarSet(AvatarPawn);
		}
	}

	// Register with the global system once we actually have a pawn avatar. We wait until this time since some globally-applied effects may require an avatar.
	if (UCustomGlobalAbilitySubsystem* GlobalAbilitySystem = UWorld::GetSubsystem<UCustomGlobalAbilitySubsystem>(GetWorld()))
	{
		GlobalAbilitySystem->RegisterASC(this);
	}

	TryActivateAbilitiesOnSpawn();
}

void UCustomAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);
	
	OnAbilityGiven.Broadcast(AbilitySpec);
}

void UCustomAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnRemoveAbility(AbilitySpec);

	// If the component is unregistered (which also trying to clear all abilities), then don't broadcast the event
	if (!bRegistered) return;

	OnAbilityRemoved.Broadcast(AbilitySpec);
}

void UCustomAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputPress ability task works.
	if (Spec.IsActive())
	{
		// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UCustomAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputRelease ability task works.
	if (Spec.IsActive())
	{
		// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputReleased event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

UCustomAbilitySystemComponent* UCustomAbilitySystemComponent::GetCustomAbilityComponent(const AActor* Actor)
{
	return Cast<UCustomAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor));
}

void UCustomAbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility)
{
	ABILITYLIST_SCOPE_LOCK();

	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (!AbilitySpec.IsActive())
		{
			continue;
		}

		UCustomGameplayAbility* AbilityCDO = Cast<UCustomGameplayAbility>(AbilitySpec.Ability);
		if (!AbilityCDO)
		{
			UE_LOG(LogCustom, Error, TEXT("CancelAbilitiesByFunc: Non-CustomGameplayAbility %s was Granted to ASC. Skipping."), *AbilitySpec.Ability.GetName())
			continue;
		}

		if (AbilityCDO->InstancingPolicy != EGameplayAbilityInstancingPolicy::NonInstanced)
		{
			// Cancel all the spawned instances, not the CDO.
			TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
			for (UGameplayAbility* Instance : Instances)
			{
				UCustomGameplayAbility* CustomAbilityInstance = Cast<UCustomGameplayAbility>(Instance);

				if (ShouldCancelFunc(CustomAbilityInstance, AbilitySpec.Handle))
				{
					if (CustomAbilityInstance->CanBeCanceled())
					{
						CustomAbilityInstance->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(),
							CustomAbilityInstance->GetCurrentActivationInfoRef(), bReplicateCancelAbility);
					}
					else
					{
						UE_LOG(LogCustom, Error, TEXT("CancelAbilitiesByFunc: Can't cancel ability [%s] because CanBeCanceled is false."), *CustomAbilityInstance->GetName());
					}
				}
			}
		}
		else
		{
			// Cancel the non-instanced ability CDO.
			if (ShouldCancelFunc(AbilityCDO, AbilitySpec.Handle))
			{
				// Non-instanced abilities can always be canceled
				check(AbilityCDO->CanBeCanceled());
				AbilityCDO->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), FGameplayAbilityActivationInfo(), bReplicateCancelAbility);
			}
		}
	}
}

void UCustomAbilitySystemComponent::UpgradeAbilityLevel(const FGameplayTag& AbilityTag,
	EAbilityUpgradeMethod UpgradeMethod, const int32 UpgradeAmount)
{
	if (FGameplayAbilitySpec* AbilitySpec = AFL::GetAbilitySpecFromTag(this, AbilityTag))
	{
		Internal_UpgradeAbilityLevel(*AbilitySpec, UpgradeMethod, UpgradeAmount);
	}
}

void UCustomAbilitySystemComponent::UpgradeAbilityLevel(FGameplayAbilitySpec& AbilitySpec,
	EAbilityUpgradeMethod UpgradeMethod, const int32 UpgradeAmount)
{
	Internal_UpgradeAbilityLevel(AbilitySpec, UpgradeMethod, UpgradeAmount);
}

void UCustomAbilitySystemComponent::UpgradeAbilityLevel(FGameplayAbilitySpec* AbilitySpec,
	EAbilityUpgradeMethod UpgradeMethod, const int32 UpgradeAmount)
{
	if (!AbilitySpec) return;

	Internal_UpgradeAbilityLevel(*AbilitySpec, UpgradeMethod, UpgradeAmount);
}

void UCustomAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag, const float UpgradeAmount)
{
	if (!CanUpgradeAttribute(AttributeTag) || UpgradeAmount <= 0) return;

	Server_UpgradeAttribute(AttributeTag, UpgradeAmount);
}

void UCustomAbilitySystemComponent::SetAttributeValue(const FGameplayTag& AttributeTag, const float NewValue)
{
	const FGameplayAttribute& Attribute = GetAttributeFromTag(AttributeTag);
	if (!Attribute.IsValid()) return;

	Server_SetAttributeValue(Attribute, NewValue);
}

void UCustomAbilitySystemComponent::SetAttributeValues(const TMap<FGameplayTag, float>& AttributeValueMap)
{
	TArray<FGameplayTag> AttributeTags;
	TArray<float> AttributeValues;
	AttributeValueMap.GenerateKeyArray(AttributeTags);
	AttributeValueMap.GenerateValueArray(AttributeValues);
	
	Server_SetAttributeValues(AttributeTags, AttributeValues);
}

void UCustomAbilitySystemComponent::GiveAbilityByTag(const FGameplayTag& AbilityTag, const int32 Level, const FGameplayTag& OptionalInputTag)
{
	if (!AbilityTag.IsValid()) return;

	// If the ability is already given, then switch input tag if valid.
	// Or swap if there's another ability with the same input tag.
	if (FGameplayAbilitySpec* AbilitySpec = AFL::GetAbilitySpecFromTag(this, AbilityTag))
	{
		if (OptionalInputTag.IsValid())
		{
			// If this ability is having input tag, and there's another ability with the same new input tag, then swap them.
			const FGameplayTag& OldInput = AFL::GetInputTagFromSpec(*AbilitySpec);
			FGameplayAbilitySpec* AbilitySpecWithSameInput = AFL::GetAbilitySpecFromInput(this, OptionalInputTag);
			if (OldInput.IsValid() && AbilitySpecWithSameInput)
			{
				SetInputTagForGivenAbility(AbilitySpecWithSameInput, OldInput);
			}

			// Now assign the new input tag for this ability.
			SetInputTagForGivenAbility(AbilityTag, AbilitySpec, OptionalInputTag);
		}
		
		return;
	}

	// Load with Asset Manager
	FPrimaryAssetId AbilityID = AFL::MakeAbilityAssetId(AbilityTag);
	const TArray<FName> Bundles = {"Ability"};
	const auto Handle = GetAssetManager().LoadPrimaryAsset(AbilityID, Bundles,
		FStreamableDelegate::CreateWeakLambda(this, [this, AbilityID, Level, OptionalInputTag] ()
		{
			// Get the ability data
			const UCustomAbilityInfoDataAsset* AbilityData = GetAssetManager().GetPrimaryAssetObject<UCustomAbilityInfoDataAsset>(AbilityID);
			const TSubclassOf<UCustomGameplayAbility> AbilityClass = AbilityData->Ability.Get();
			if (!AbilityClass) return;

			// Make the ability spec
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, Level);

			// Add the input tag
			const FGameplayTag& PotentialInputTag = OptionalInputTag.IsValid() ? OptionalInputTag : AbilityData->StartingInputTag;
			if (PotentialInputTag.IsValid())
			{
				// Replace the abilities with same input tag
				RemoveAbilitiesWithInputTag(PotentialInputTag);

				// Now assign the input tag
				AbilitySpec.DynamicAbilityTags.AddTag(PotentialInputTag);
			}

			// Finally, give the ability
			GiveAbility(AbilitySpec);
		}));
}

void UCustomAbilitySystemComponent::ListenToAbilityGivenEvent(const FAbilityGiven& Delegate)
{
	ABILITYLIST_SCOPE_LOCK();
	
	OnAbilityGiven.AddUnique(Delegate);

	// Still broadcast the event if the ability was already given
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		OnAbilityGiven.Broadcast(AbilitySpec);
	}
}

void UCustomAbilitySystemComponent::RemoveAbilityByTag(const FGameplayTag& AbilityTag)
{
	ABILITYLIST_SCOPE_LOCK();
	
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (AFL::IsAbilityTagInSpec(Spec, AbilityTag))
		{
			ClearAbility(Spec.Handle);
		}
	}
}

void UCustomAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.Ability && AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			InputPressedSpecHandles.Emplace(AbilitySpec.Handle);
			InputHeldSpecHandles.Emplace(AbilitySpec.Handle);
		}
	}
}

void UCustomAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.Ability && AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			InputReleasedSpecHandles.Emplace(AbilitySpec.Handle);
			InputHeldSpecHandles.Remove(AbilitySpec.Handle);
		}
	}
}

void UCustomAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	/*if (HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked))
	{
		ClearAllInputHandles();
		return;
	}*/
	
	// Mark "static" so it doesn't get reallocated every call
	static TSet<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();
	
	// Process all abilities that activate when the input is held.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UCustomGameplayAbility* CustomAbilityCDO = Cast<UCustomGameplayAbility>(AbilitySpec->Ability);
				if (CustomAbilityCDO && CustomAbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.Emplace(AbilitySpec->Handle);
				}
			}
		}
	}
	
	// Process all abilities that had their input pressed this frame.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UCustomGameplayAbility* CustomAbilityCDO = Cast<UCustomGameplayAbility>(AbilitySpec->Ability);

					if (CustomAbilityCDO && CustomAbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::OnInputTriggered)
					{
						AbilitiesToActivate.Emplace(AbilitySpec->Handle);
					}
				}
			}
		}
	}
	
	// Try to activate all the abilities that are from presses and holds.
	// We do it all at once so that held inputs don't activate the ability
	// and then also send a input event to the ability because of the press.
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}
	
	// Process all abilities that had their input released this frame.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}
	
	// Clear the cached ability handles.
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UCustomAbilitySystemComponent::ClearAllInputHandles()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void UCustomAbilitySystemComponent::ClearAbilityInputTag(FGameplayAbilitySpec* AbilitySpec, const bool bMarkSpecDirty)
{
	if (!AbilitySpec) return;

	const FGameplayTag& Input = AFL::GetInputTagFromSpec(*AbilitySpec);
	if (Input.IsValid())
	{
		AbilitySpec->DynamicAbilityTags.RemoveTag(Input);
		if (bMarkSpecDirty)
		{
			MarkAbilitySpecDirty(*AbilitySpec);
		}
	}
}

void UCustomAbilitySystemComponent::RemoveAbilitiesWithInputTag(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid() || !InputTag.MatchesTag(CustomTags::Input)) return;

	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			ClearAbility(AbilitySpec.Handle);
		}
	}
}

void UCustomAbilitySystemComponent::TryActivateAbilitiesOnSpawn()
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (const UCustomGameplayAbility* CustomAbilityCDO = Cast<UCustomGameplayAbility>(AbilitySpec.Ability))
		{
			CustomAbilityCDO->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), AbilitySpec);
		}
	}
}

void UCustomAbilitySystemComponent::Internal_UpgradeAbilityLevel(FGameplayAbilitySpec& AbilitySpec,
	EAbilityUpgradeMethod UpgradeMethod, const int32 UpgradeAmount)
{
	if (UpgradeAmount <= 0) return;
	
	if (!IsOwnerActorAuthoritative())
	{
		UE_LOG(LogCustom, Error, TEXT("Internal_UpgradeAbilityLevel called on ability %s on the client, not allowed!"), *AbilitySpec.Ability->GetName());
		return;
	}
	
	// Cache this tag of the ability
	const FGameplayTag AbilityTag = AFL::GetFirstAbilityTagFromSpec(AbilitySpec);
	
	// Upgrade
	switch (UpgradeMethod)
	{
	case EAbilityUpgradeMethod::Default:
		{
			AbilitySpec.Level += UpgradeAmount;

			// Mark dirty to it can be instantly replicated
			MarkAbilitySpecDirty(AbilitySpec);
			
			break;
		}
	case EAbilityUpgradeMethod::CancelAbility:
		{
			const FGameplayAbilitySpec NewSpec(
				AbilitySpec.Ability,
				AbilitySpec.Level + UpgradeAmount,
				AbilitySpec.InputID,
				AbilitySpec.SourceObject.Get());
			ClearAbility(AbilitySpec.Handle);
			GiveAbility(NewSpec);
			
			break;
		}
	default:;
	}

	// Broadcast the event
	Multicast_BroadcastOnAbilityUpgraded(AbilityTag, AbilitySpec.Level);
}

void UCustomAbilitySystemComponent::Multicast_BroadcastOnAbilityUpgraded_Implementation(const FGameplayTag& AbilityTag,
	const int UpgradedAmount)
{
	OnAbilityLevelUpgraded.Broadcast(AbilityTag, UpgradedAmount);
}

void UCustomAbilitySystemComponent::Server_UpgradeAttribute_Implementation(const FGameplayTag& AttributeTag, const float UpgradeAmount)
{
	const FGameplayAttribute& UpgradingAttribute = GetAttributeFromTag(AttributeTag);
	if (!UpgradingAttribute.IsValid()) return;

	// Create a dynamic instant Gameplay Effect
	UGameplayEffect* DynamicGE = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("Upgrade Attribute")));
	DynamicGE->DurationPolicy = EGameplayEffectDurationType::Instant;

	// Upgrade settings
	DynamicGE->Modifiers.SetNum(1);
	DynamicGE->Modifiers[0].ModifierOp = EGameplayModOp::Additive;
	DynamicGE->Modifiers[0].ModifierMagnitude = FScalableFloat(UpgradeAmount);
	DynamicGE->Modifiers[0].Attribute = UpgradingAttribute;

	// Apply
	ApplyGameplayEffectToSelf(DynamicGE, 1.0f, MakeEffectContext());

	OnAttributeUpgraded.Broadcast(AttributeTag, UpgradeAmount);
}

bool UCustomAbilitySystemComponent::CanUpgradeAttribute(const FGameplayTag& AttributeTag) const
{
	return true;
}

void UCustomAbilitySystemComponent::Server_SetAttributeValue_Implementation(const FGameplayAttribute& Attribute, const float NewValue)
{
	if (!Attribute.IsValid()) return;

	// Create a dynamic instant Gameplay Effect
	UGameplayEffect* DynamicGE = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("Set Attribute")));
	DynamicGE->DurationPolicy = EGameplayEffectDurationType::Instant;

	// Override settings
	DynamicGE->Modifiers.SetNum(1);
	DynamicGE->Modifiers[0].ModifierOp = EGameplayModOp::Override;
	DynamicGE->Modifiers[0].ModifierMagnitude = FScalableFloat(NewValue);
	DynamicGE->Modifiers[0].Attribute = Attribute;

	// Apply
	ApplyGameplayEffectToSelf(DynamicGE, 1.0f, MakeEffectContext());
}

void UCustomAbilitySystemComponent::Server_SetAttributeValues_Implementation(
	const TArray<FGameplayTag>& AttributeTags, const TArray<float>& AttributeValues)
{
	if (AttributeTags.Num() <= 0 || AttributeValues.Num() <= 0) return;
	if (AttributeTags.Num() != AttributeValues.Num()) return;

	// Create a dynamic instant Gameplay Effect
	UGameplayEffect* DynamicGE = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("Upgrade Attribute")));
	DynamicGE->DurationPolicy = EGameplayEffectDurationType::Instant;
	DynamicGE->Modifiers.Reset();

	TMap<FGameplayTag, float> AttributeTagsMap;
	for (int I = 0; I < AttributeTags.Num(); I++)
	{
		AttributeTagsMap.Add(AttributeTags[I], AttributeValues[I]);
	}
	
	for (const TPair<FGameplayTag, float>& Pair : AttributeTagsMap)
	{
		const FGameplayAttribute& Attribute = GetAttributeFromTag(Pair.Key);
		if (!Attribute.IsValid()) continue;

		const float AttributeValue = Pair.Value;

		// Make Modifier
		FGameplayModifierInfo Modifier;
		Modifier.ModifierOp = EGameplayModOp::Override;
		Modifier.ModifierMagnitude = FScalableFloat(AttributeValue);
		Modifier.Attribute = Attribute;

		// Add Modifier to list in DynamicGE
		DynamicGE->Modifiers.Emplace(Modifier);
	}

	// Apply
	ApplyGameplayEffectToSelf(DynamicGE, 1.0f, MakeEffectContext());
}

void UCustomAbilitySystemComponent::Client_BroadcastAbilityInputChanged_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& NewInputTag)
{
	OnAbilityInputTagChanged.Broadcast(AbilityTag, AbilitySpec, NewInputTag);
}

void UCustomAbilitySystemComponent::SetInputTagForGivenAbility(FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& InputTag)
{
	if (!AbilitySpec || !InputTag.IsValid()) return;
	
	ClearAbilityInputTag(AbilitySpec, false);
	AbilitySpec->DynamicAbilityTags.AddTag(InputTag);
	MarkAbilitySpecDirty(*AbilitySpec);

	const FGameplayTag& AbilityTag = AFL::GetFirstAbilityTagFromSpec(*AbilitySpec);
	OnAbilityInputTagChanged.Broadcast(AbilityTag, *AbilitySpec, InputTag);

	// Also broadcast to the Client, since this function is usually called on the Server
	Client_BroadcastAbilityInputChanged(AbilityTag, *AbilitySpec, InputTag);
}

void UCustomAbilitySystemComponent::SetInputTagForGivenAbility(const FGameplayTag& AbilityTag,
	FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& InputTag) 
{
	if (!AbilitySpec || !InputTag.IsValid()) return;
	
	ClearAbilityInputTag(AbilitySpec, false);
	AbilitySpec->DynamicAbilityTags.AddTag(InputTag);
	MarkAbilitySpecDirty(*AbilitySpec);

	OnAbilityInputTagChanged.Broadcast(AbilityTag, *AbilitySpec, InputTag);
	
	// Also broadcast to the Client, since this function is usually called on the Server
	Client_BroadcastAbilityInputChanged(AbilityTag, *AbilitySpec, InputTag);
}

const FGameplayAttribute& UCustomAbilitySystemComponent::GetAttributeFromTag(const FGameplayTag& AttributeTag)
{
	return UCustomAbilitySystemGlobals::GetAttributeFromTag(AttributeTag);
}

UAssetManager& UCustomAbilitySystemComponent::GetAssetManager() const
{
	check(this);
	return UAssetManager::Get();
}
