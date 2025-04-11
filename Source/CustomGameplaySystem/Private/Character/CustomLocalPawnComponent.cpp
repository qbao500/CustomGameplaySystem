// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CustomLocalPawnComponent.h"

#include "CustomGameplayTags.h"
#include "Character/CustomCorePawnComponent.h"
#include "Character/CustomPawnData.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Components/GameFrameworkComponentManager.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "GameFramework/PlayerState.h"
#include "Input/CustomPlayerController.h"
#include "Player/CustomPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomLocalPawnComponent)

const FName UCustomLocalPawnComponent::NAME_ActorFeatureName("LocalPawn");
const FName UCustomLocalPawnComponent::NAME_BindInputsNow("BindInputsNow");

UCustomLocalPawnComponent* UCustomLocalPawnComponent::FindLocalPawnComponent(AActor* Actor)
{
	const APawn* Pawn = FindPawnFromActor(Actor);
	return FindLocalPawnComponent(Pawn);
}

UCustomLocalPawnComponent* UCustomLocalPawnComponent::FindLocalPawnComponent(const APawn* Pawn)
{
	return Pawn ? Pawn->FindComponentByClass<UCustomLocalPawnComponent>() : nullptr;
}

FName UCustomLocalPawnComponent::GetFeatureName() const
{
	return NAME_ActorFeatureName;
}

void UCustomLocalPawnComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UCustomCorePawnComponent::NAME_ActorFeatureName)
	{
		CheckDefaultInitialization();
	}
}

bool UCustomLocalPawnComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == CustomTags::InitState_Spawned)
	{
		// As long as we have a real pawn, let us transition
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == CustomTags::InitState_Spawned && DesiredState == CustomTags::InitState_DataAvailable)
	{
		// The player state is required.
		if (!GetPlayerState<ACustomPlayerState>())
		{
			return false;
		}

		// If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();

			const bool bHasControllerPairedWithPS = (Controller != nullptr) && \
				(Controller->PlayerState != nullptr) && \
				(Controller->PlayerState->GetOwner() == Controller);

			if (!bHasControllerPairedWithPS)
			{
				return false;
			}
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const bool bIsBot = Pawn->IsBotControlled();

		if (bIsLocallyControlled && !bIsBot)
		{
			ACustomPlayerController* CustomPC = GetController<ACustomPlayerController>();

			// The input component and local player is required when locally controlled.
			if (!Pawn->InputComponent || !CustomPC || !CustomPC->GetLocalPlayer())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == CustomTags::InitState_DataAvailable && DesiredState == CustomTags::InitState_DataInitialized)
	{
		// Wait for player state and CorePawnComponent
		const ACustomPlayerState* CustomPS = GetPlayerState<ACustomPlayerState>();
		const FGameplayTag CoreState = Manager->GetInitStateForFeature(Pawn, UCustomCorePawnComponent::NAME_ActorFeatureName);
		const bool bReached = Manager->IsInitStateAfterOrEqual(CoreState, CustomTags::InitState_DataInitialized);

		return CustomPS && bReached;
	}
	else if (CurrentState == CustomTags::InitState_DataInitialized && DesiredState == CustomTags::InitState_GameplayReady)
	{
		// TODO add ability initialization checks?
		return true;
	}

	return false;
}

void UCustomLocalPawnComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (CurrentState == CustomTags::InitState_DataAvailable && DesiredState == CustomTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		ACustomPlayerState* CustomPS = GetPlayerState<ACustomPlayerState>();
		if (!ensure(Pawn && CustomPS))
		{
			return;
		}

		const UCustomPawnData* PawnData = nullptr;
		if (UCustomCorePawnComponent* CorePawnComp = UCustomCorePawnComponent::FindCorePawnComponent(Pawn))
		{
			PawnData = CorePawnComp->GetPawnData();
			// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
			// The ability system component and attribute sets live on the player state.
			CorePawnComp->InitializeAbilitySystem(CustomPS->GetCustomAbilitySystemComponent(), CustomPS);
		}

		// Handle setup Player Controller input
		if (Pawn->IsLocallyControlled())
		{
			if (ACustomPlayerController* CustomPC = GetController<ACustomPlayerController>())
			{
				CustomPC->InitializePlayerInputFromConfig(PawnData->InputConfig);

				if (!ensure(!bReadyToBindExtraInputs))
				{
					bReadyToBindExtraInputs = true;
				}

				UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(CustomPC, NAME_BindInputsNow);
				UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(Pawn, NAME_BindInputsNow);
			}
		}
	}
}

void UCustomLocalPawnComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for when CustomCorePawnComponent changes init state
	BindOnActorInitStateChanged(UCustomCorePawnComponent::NAME_ActorFeatureName, FGameplayTag(), false);
}
