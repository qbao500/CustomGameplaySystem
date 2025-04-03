// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CustomCorePawnComponent.h"

#include "CustomGameplayTags.h"
#include "CustomLogChannels.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "GAS/Components/CustomAbilitySystemComponent.h"
#include "Player/CustomPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomCorePawnComponent)

const FName UCustomPawnComponent_Base::NAME_ActorFeatureName("CorePawn");

UCustomCorePawnComponent::UCustomCorePawnComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UCustomCorePawnComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

UCustomCorePawnComponent* UCustomCorePawnComponent::FindCorePawnComponent(const AActor* Actor)
{
	if (!Actor) return nullptr;

	const AActor* CheckingActor = Actor;

	if (const APlayerState* PlayerState = Cast<APlayerState>(CheckingActor))
	{
		CheckingActor = PlayerState->GetPawn();
	}
	else if (const AController* Controller = Cast<AController>(CheckingActor))
	{
		CheckingActor = Controller->GetPawn();
	}

	if (!CheckingActor) return nullptr;
	
	return CheckingActor->FindComponentByClass<UCustomCorePawnComponent>();
}

UCustomCorePawnComponent* UCustomCorePawnComponent::FindCorePawnComponent(const APawn* Pawn)
{
	return Pawn ? Pawn->FindComponentByClass<UCustomCorePawnComponent>() : nullptr;
}

bool UCustomCorePawnComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	if (CurrentState == CustomTags::InitState_DataAvailable && DesiredState == CustomTags::InitState_DataInitialized)
	{
		const ACustomPlayerState* CustomPS = GetPlayerState<ACustomPlayerState>();
		return CustomPS != nullptr;
	}
	
	return Super::CanChangeInitState(Manager, CurrentState, DesiredState);
}

void UCustomCorePawnComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (CurrentState == CustomTags::InitState_DataAvailable && DesiredState == CustomTags::InitState_DataInitialized)
	{
		const APawn* Pawn = GetPawn<APawn>();
		ACustomPlayerState* CustomPS = GetPlayerState<ACustomPlayerState>();
		if (!ensure(Pawn && CustomPS))
		{
			return;
		}

		// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
		// The ability system component and attribute sets live on the player state.
		InitializeAbilitySystem(CustomPS->GetCustomAbilitySystemComponent(), CustomPS);
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

	APawn* Pawn = GetPawnChecked<APawn>();
	const AActor* ExistingAvatar = InASC->GetAvatarActor();

	UE_LOG(LogCustom, Verbose, TEXT("Setting up ASC [%s] on pawn [%s] owner [%s], existing [%s] "), *GetNameSafe(InASC), *GetNameSafe(Pawn), *GetNameSafe(InOwnerActor), *GetNameSafe(ExistingAvatar));

	if ((ExistingAvatar != nullptr) && (ExistingAvatar != Pawn))
	{
		UE_LOG(LogCustom, Log, TEXT("Existing avatar (authority=%d)"), ExistingAvatar->HasAuthority() ? 1 : 0);

		// There is already a pawn acting as the ASC's avatar, so we need to kick it out
		// This can happen on clients if they're lagged: their new pawn is spawned + possessed before the dead one is removed
		if (!ExistingAvatar->HasAuthority()) return;;

		if (UCustomCorePawnComponent* OtherExtensionComponent = FindCustomPawnComponent<UCustomCorePawnComponent>(ExistingAvatar))
		{
			OtherExtensionComponent->UninitializeAbilitySystem();
		}
	}

	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);

	BroadcastAbilitySystemInitialized();
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

void UCustomCorePawnComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeAbilitySystem();
	
	Super::EndPlay(EndPlayReason);
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
