// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CustomCorePawnComponent.h"

#include "AbilitySystemGlobals.h"
#include "CustomGameplayTags.h"
#include "CustomLogChannels.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "GAS/Components/CustomAbilitySystemComponent.h"
#include "Misc/DataValidation.h"
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
	check(Manager);

	const APawn* Pawn = GetPawn<APawn>();
	if (!CurrentState.IsValid() && DesiredState == CustomTags::InitState_Spawned)
	{
		// As long as we are on a valid pawn, we count as spawned
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == CustomTags::InitState_Spawned && DesiredState == CustomTags::InitState_DataAvailable)
	{
		// Pawn data is required.
		/*if (!PawnData)
		{
			return false;
		}*/

		// If we're authority or autonomous, we need to wait for a Controller with registered ownership of the PlayerState (must for PlayerController).
		// Also for local PlayerController, we need both InputComponent and LocalPlayer to be valid.
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			const AController* Controller = GetController<AController>();
			if (!Controller) return false;
			
			// If it's Player, then PlayerState is a must. AI can optionally have PlayerState.
			const APlayerState* PlayerState = Controller->PlayerState;
			
			// Handle for Player
			if (const APlayerController* PC = Cast<APlayerController>(Controller))
			{
				const bool bHasControllerPairedWithPS = IsValid(PlayerState) && PlayerState->GetOwner() == Controller;
				const bool bHasValidInput = Pawn->InputComponent && PC->GetLocalPlayer();

				return bHasControllerPairedWithPS && bHasValidInput;
			}
			
			// AI can have PlayerState, but it's not a must
			if (PlayerState)
			{
				return PlayerState->GetOwner() == Controller;
			}
		}

		return true;
	}
	
	if (CurrentState == CustomTags::InitState_DataAvailable && DesiredState == CustomTags::InitState_DataInitialized)
	{
		return true;
	}

	if (CurrentState == CustomTags::InitState_DataInitialized && DesiredState == CustomTags::InitState_GameplayReady)
	{
		return true;
	}
	
	return Super::CanChangeInitState(Manager, CurrentState, DesiredState);
}

void UCustomCorePawnComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (CurrentState == CustomTags::InitState_DataAvailable && DesiredState == CustomTags::InitState_DataInitialized)
	{
		// At this point, we must have a valid Pawn and Controller
		// For Player (and optionally AI), PlayerState must be valid too
		APawn* Pawn = GetPawn<APawn>();
		const AController* Controller = GetController<AController>();
		if (!ensure(Pawn && Controller))
		{
			return;
		}

		// The PlayerState holds the persistent data (state that persists across deaths and multiple Pawns).
		// The AbilitySystemComponent and AttributeSets live on the PlayerState.
		if (ACustomPlayerState* CustomPS = GetPlayerState<ACustomPlayerState>())
		{
			InitializeAbilitySystem(CustomPS->GetCustomAbilitySystemComponent(), CustomPS);
		}
		else
		{
			// If PlayerState is not valid, it's most likely this is an AI
			// The AI Pawn must have a valid CustomAbilitySystemComponent
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
		if (!ExistingAvatar->HasAuthority()) return;

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

void UCustomCorePawnComponent::OnRep_PawnData()
{
	CheckDefaultInitialization();
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

EDataValidationResult UCustomCorePawnComponent::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	/*if (!PawnData)
	{
		Result = EDataValidationResult::Invalid;
		Context.AddError(FText::FromString("Please assign PawnData!!!"));
	}*/

	return Result;
}
