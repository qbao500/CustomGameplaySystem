// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CustomCorePawnComponent.h"

#include "AbilitySystemGlobals.h"
#include "CustomGameplayTags.h"
#include "CustomLogChannels.h"
#include "Character/CustomPawnData.h"
#include "Components/GameFrameworkComponentManager.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "GAS/CustomAbilitySet.h"
#include "GAS/Components/CustomAbilitySystemComponent.h"
#include "Misc/DataValidation.h"
#include "Net/UnrealNetwork.h"
#include "Player/CustomPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomCorePawnComponent)

const FName UCustomCorePawnComponent::NAME_ActorFeatureName("CorePawn");

UCustomCorePawnComponent::UCustomCorePawnComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Replicate for setting up Ability System Component
	SetIsReplicatedByDefault(true);
}

void UCustomCorePawnComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCustomCorePawnComponent, PawnData);
}

UCustomCorePawnComponent* UCustomCorePawnComponent::FindCorePawnComponent(AActor* Actor)
{
	const APawn* Pawn = FindPawnFromActor(Actor);
	return FindCorePawnComponent(Pawn);
}

UCustomCorePawnComponent* UCustomCorePawnComponent::FindCorePawnComponent(const APawn* Pawn)
{
	return Pawn ? Pawn->FindComponentByClass<UCustomCorePawnComponent>() : nullptr;
}

FName UCustomCorePawnComponent::GetFeatureName() const
{
	return NAME_ActorFeatureName;
}

void UCustomCorePawnComponent::CheckDefaultInitialization()
{
	// Before checking our progress, try progressing any other features we might depend on
	CheckDefaultInitializationForImplementers();
	
	Super::CheckDefaultInitialization();
}

void UCustomCorePawnComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	Super::OnActorInitStateChanged(Params);

	// If another feature is now in DataAvailable, see if we should transition to DataInitialized
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		if (Params.FeatureState == CustomTags::InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

bool UCustomCorePawnComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	
	if (!CurrentState.IsValid() && DesiredState == CustomTags::InitState_Spawned)
	{
		// As long as we are on a valid pawn, we count as spawned
		if (Pawn)
		{
			return true;
		}
	}
	
	if (CurrentState == CustomTags::InitState_Spawned && DesiredState == CustomTags::InitState_DataAvailable)
	{
		// Pawn data is required.
		if (!PawnData)
		{
			PLFL::PrintError("Require PawnData for CustomCorePawnComponent on " + GetNameSafe(Pawn));
			return false;
		}
		
		if (IsAuthorityOrLocal())
		{
			// Check for being possessed by a controller.
			if (!GetController<AController>())
			{
				return false;
			}
		}

		return true;
	}
	
	if (CurrentState == CustomTags::InitState_DataAvailable && DesiredState == CustomTags::InitState_DataInitialized)
	{
		// Transition to initialize if all features have their data available
		return Manager->HaveAllFeaturesReachedInitState(Pawn, CustomTags::InitState_DataAvailable);
	}

	if (CurrentState == CustomTags::InitState_DataInitialized && DesiredState == CustomTags::InitState_GameplayReady)
	{
		return true;
	}
	
	return false;
}

void UCustomCorePawnComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (CurrentState == CustomTags::InitState_DataAvailable && DesiredState == CustomTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawnChecked<APawn>();

		// ASC doesn't care about Simulated Proxy
		if (Pawn->GetLocalRole() == ROLE_SimulatedProxy) return;

		// The PlayerState holds the persistent data (state that persists across deaths and multiple Pawns).
		// The AbilitySystemComponent and AttributeSets live on the PlayerState.
		if (ACustomPlayerState* CustomPS = GetPlayerState<ACustomPlayerState>())
		{
			// This Player (or AI) has a valid PlayerState. Now Initialize the ASC.
			InitializeAbilitySystem(CustomPS->GetCustomAbilitySystemComponent(), CustomPS);
		}
		else
		{
			// If PlayerState is not valid, it's most likely this is an AI.
			// The AI Pawn must have a valid CustomAbilitySystemComponent.
			UCustomAbilitySystemComponent* CustomASC = CastChecked<UCustomAbilitySystemComponent>(
				UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn));
			InitializeAbilitySystem(CustomASC, Pawn);
		}
	}
}

void UCustomCorePawnComponent::HandleControllerChanged()
{
	if (AbilitySystemComponent && (AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>()))
	{
		ensure(AbilitySystemComponent->AbilityActorInfo->OwnerActor == AbilitySystemComponent->GetOwnerActor());
		if (AbilitySystemComponent->GetOwnerActor() == nullptr)
		{
			UninitializeAbilitySystem();
		}
		else
		{
			AbilitySystemComponent->RefreshAbilityActorInfo();
		}
	}
	
	Super::HandleControllerChanged();
}

void UCustomCorePawnComponent::InitializeAbilitySystem(UCustomAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	check(InASC);
	check(InOwnerActor);

	APawn* Pawn = GetPawnChecked<APawn>();

	// ASC doesn't care about Simulated Proxy
	if (Pawn->GetLocalRole() == ROLE_SimulatedProxy) return;

	if (AbilitySystemComponent == InASC)
	{
		// The ability system component hasn't changed.
		return;
	}

	if (AbilitySystemComponent)
	{
		// Clean up the old ability system component.
		UninitializeAbilitySystem();
	}
	
	const AActor* ExistingAvatar = InASC->GetAvatarActor();

	UE_LOG(LogCustom, Verbose, TEXT("Setting up ASC [%s] on pawn [%s] owner [%s], existing [%s] "), *GetNameSafe(InASC), *GetNameSafe(Pawn), *GetNameSafe(InOwnerActor), *GetNameSafe(ExistingAvatar));

	if (ExistingAvatar != nullptr && ExistingAvatar->IsA(APawn::StaticClass()) && ExistingAvatar != Pawn)
	{
		UE_LOG(LogCustom, Log, TEXT("Existing avatar (authority=%d)"), ExistingAvatar->HasAuthority() ? 1 : 0);

		// There is already a pawn acting as the ASC's avatar, so we need to kick it out
		// This can happen on clients if they're lagged: their new pawn is spawned + possessed before the dead one is removed
		if (!ExistingAvatar->HasAuthority()) return;

		if (UCustomCorePawnComponent* OtherExtensionComponent = FindCustomPawnComponent<UCustomCorePawnComponent>(ExistingAvatar))
		{
			OtherExtensionComponent->UninitializeAbilitySystem();
		}
	}

	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);
	
	BroadcastAbilitySystemInitialized();

	// This function is triggered on Server before Client, because Client needs to wait for Player State and Controller.
	// So here, when Client is finished, it will now ask Server to grant abilities.
	// This is to prevent Server grant abilities before Client is ready. Otherwise, Client could have null Avatar.
	if (Pawn->IsLocallyControlled())
	{
		Server_GrantAbilitySets();
	}
}

void UCustomCorePawnComponent::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		FGameplayTagContainer AbilityTypesToIgnore;
		AbilityTypesToIgnore.AddTag(CustomTags::Ability_Behavior_SurvivesDeath);

		AbilitySystemComponent->CancelAbilities(nullptr, &AbilityTypesToIgnore);
		AbilitySystemComponent->ClearAllInputHandles();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing
			AbilitySystemComponent->ClearActorInfo();
		}

		OnAbilitySystemUninitialized.Broadcast();
	}

	AbilitySystemComponent = nullptr;
}

void UCustomCorePawnComponent::OnAbilitySystemInitialized_RegisterAndCall(const FAbilityComponentInitialized& Delegate, const bool bOnceOnly)
{
	OnAbilitySystemInitialized.AddUnique(Delegate);

	if (bOnceOnly)
	{
		ListenOnceObjects.Emplace(Delegate.GetUObject());
	}

	if (AbilitySystemComponent)
	{
		BroadcastAbilitySystemInitialized();
	}
}

void UCustomCorePawnComponent::OnAbilitySystemUninitialized_Register(const FSimpleMulticastDelegate::FDelegate& Delegate)
{
	if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemUninitialized.Add(Delegate);
	}
}

const UCustomPawnData* UCustomCorePawnComponent::GetPawnData() const
{
	return PawnData;
}

void UCustomCorePawnComponent::BeginPlay()
{
	// Listen for changes to all features
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);
	
	Super::BeginPlay();
}

void UCustomCorePawnComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeAbilitySystem();
	
	Super::EndPlay(EndPlayReason);
}

void UCustomCorePawnComponent::OnRep_PawnData()
{
	CheckDefaultInitialization();
}

void UCustomCorePawnComponent::Server_GrantAbilitySets_Implementation()
{
	check(PawnData);
	check(AbilitySystemComponent);

	for (TObjectPtr<UCustomAbilitySet> AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}
}

void UCustomCorePawnComponent::BroadcastAbilitySystemInitialized()
{
	OnAbilitySystemInitialized.Broadcast(AbilitySystemComponent);
	
	// Copy and clear first, in case the array is modified during the removal.
	TSet<const UObject*> ListenOnceDelegatesCopy = ListenOnceObjects;
	ListenOnceObjects.Empty();
	for (const UObject* Object : ListenOnceDelegatesCopy)
	{
		OnAbilitySystemInitialized.RemoveAll(Object);
	}
}

bool UCustomCorePawnComponent::IsAuthorityOrLocal() const
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return false;
	}

	const bool bHasAuthority = Pawn->HasAuthority();
	const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
	return bHasAuthority || bIsLocallyControlled;
}

EDataValidationResult UCustomCorePawnComponent::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	if (!PawnData)
	{
		Result = EDataValidationResult::Invalid;
		Context.AddError(FText::FromString("Please assign PawnData!!!"));
	}

	return Result;
}
