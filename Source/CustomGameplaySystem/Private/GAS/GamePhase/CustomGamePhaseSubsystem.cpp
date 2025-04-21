// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GamePhase/CustomGamePhaseSubsystem.h"

#include "CustomLogChannels.h"
#include "GameFramework/GameStateBase.h"
#include "GAS/Components/CustomAbilitySystemComponent.h"
#include "GAS/GamePhase/CustomGamePhaseAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomGamePhaseSubsystem)

UCustomGamePhaseSubsystem* UCustomGamePhaseSubsystem::Get(const UObject* WorldContextObject)
{
	return WorldContextObject->GetWorld()->GetSubsystem<UCustomGamePhaseSubsystem>();
}

void UCustomGamePhaseSubsystem::StartPhase(const TSubclassOf<UCustomGamePhaseAbility>& PhaseAbility,
	const FGamePhaseDelegate& PhaseEndedCallback)
{
	if (!PhaseAbility) return;

	const UCustomGamePhaseAbility* PhaseCDO = PhaseAbility.GetDefaultObject();
	if (!PhaseCDO) return;

	// Don't start the phase if it is already active
	if (IsPhaseActive(PhaseCDO->GetGamePhaseTag()))
	{
		return;
	}
	
	UCustomAbilitySystemComponent* GameStateASC = GetGameStateASC();
	if (ensure(GameStateASC))
	{
		const FGameplayAbilitySpec Spec = FGameplayAbilitySpec(PhaseAbility, 1, INDEX_NONE, this);
		const FGameplayAbilitySpecHandle SpecHandle = GameStateASC->GiveAbility(Spec);
		const FGameplayAbilitySpec* FoundSpec = GameStateASC->FindAbilitySpecFromHandle(SpecHandle);

		if (FoundSpec && FoundSpec->IsActive())
		{
			FGamePhaseEntry& Entry = ActivePhaseMap.FindOrAdd(SpecHandle);
			Entry.PhaseEndedCallback = PhaseEndedCallback;
		}
		else
		{
			// Fail to activate for some reason, so we can trigger the callback for Phase Ended
			PhaseEndedCallback.ExecuteIfBound(nullptr);
		}
	}
}

void UCustomGamePhaseSubsystem::WhenPhaseStartsOrIsActive(UObject* Listener, const FGameplayTag& PhaseTag,
	const EPhaseTagMatchType MatchType, const FGamePhaseTagDelegate& WhenPhaseActive, const bool bListenOnce)
{
	if (!PhaseTag.IsValid() || !Listener) return;
	
	// If phase is already active, then don't add an observer, just call the delegate
	if (IsPhaseActive(PhaseTag))
	{
		WhenPhaseActive.ExecuteIfBound(PhaseTag);
		return;
	}
	
	FPhaseObserveInfo Observer;
	Observer.PhaseTag = PhaseTag;
	Observer.MatchType = MatchType;
	Observer.PhaseCallback = WhenPhaseActive;
	Observer.bObserveOnce = bListenOnce;
	PhaseStartObservers.Emplace(Listener, Observer);
}

void UCustomGamePhaseSubsystem::WhenPhaseEnds(UObject* Listener, const FGameplayTag& PhaseTag,
	const EPhaseTagMatchType MatchType,	const FGamePhaseTagDelegate& WhenPhaseEnd, const bool bListenOnce)
{
	if (!PhaseTag.IsValid() || !Listener) return;
	
	FPhaseObserveInfo Observer;
	Observer.PhaseTag = PhaseTag;
	Observer.MatchType = MatchType;
	Observer.PhaseCallback = WhenPhaseEnd;
	Observer.bObserveOnce = bListenOnce;
	PhaseEndObservers.Emplace(Listener, Observer);
}

void UCustomGamePhaseSubsystem::K2_StartPhase(TSubclassOf<UCustomGamePhaseAbility> PhaseAbility,
	const FGamePhaseDynamicDelegate& PhaseEnded)
{
	if (!PhaseAbility) return;
	
	const FGamePhaseDelegate EndedDelegate = FGamePhaseDelegate::CreateWeakLambda(const_cast<UObject*>(PhaseEnded.GetUObject()),
		[PhaseEnded](const UCustomGamePhaseAbility* PhaseAbility)
		{
			PhaseEnded.ExecuteIfBound(PhaseAbility);
		});

	StartPhase(PhaseAbility, EndedDelegate);
}

void UCustomGamePhaseSubsystem::K2_WhenPhaseStartsOrIsActive(UObject* Listener, const FGameplayTag& PhaseTag,
	const EPhaseTagMatchType MatchType,	FGamePhaseTagDynamicDelegate WhenPhaseActive, const bool bListenOnce)
{
	if (!PhaseTag.IsValid() || !Listener) return;
	
	const FGamePhaseTagDelegate ActiveDelegate = FGamePhaseTagDelegate::CreateWeakLambda(WhenPhaseActive.GetUObject(),
		[WhenPhaseActive](const FGameplayTag& PhaseTag)
		{
			WhenPhaseActive.ExecuteIfBound(PhaseTag);
		});
	
	WhenPhaseStartsOrIsActive(Listener, PhaseTag, MatchType, ActiveDelegate, bListenOnce);
}

void UCustomGamePhaseSubsystem::K2_WhenPhaseEnds(UObject* Listener, const FGameplayTag& PhaseTag,
	const EPhaseTagMatchType MatchType,	FGamePhaseTagDynamicDelegate WhenPhaseEnd, const bool bListenOnce)
{
	if (!PhaseTag.IsValid() || !Listener) return;
	
	const FGamePhaseTagDelegate EndedDelegate = FGamePhaseTagDelegate::CreateWeakLambda(WhenPhaseEnd.GetUObject(),
		[WhenPhaseEnd](const FGameplayTag& PhaseTag)
		{
			WhenPhaseEnd.ExecuteIfBound(PhaseTag);
		});

	WhenPhaseEnds(Listener, PhaseTag, MatchType, EndedDelegate, bListenOnce);
}

bool UCustomGamePhaseSubsystem::IsPhaseActive(const FGameplayTag& PhaseTag) const
{
	for (const auto& KVP : ActivePhaseMap)
	{
		const FGamePhaseEntry& PhaseEntry = KVP.Value;
		if (PhaseEntry.PhaseTag.MatchesTag(PhaseTag))
		{
			return true;
		}
	}

	return false;
}

bool UCustomGamePhaseSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UCustomGamePhaseSubsystem::OnBeginPhase(const UCustomGamePhaseAbility* PhaseAbility,
	const FGameplayAbilitySpecHandle PhaseAbilityHandle)
{
	check(PhaseAbility);

	const FGameplayTag& IncomingPhaseTag = PhaseAbility->GetGamePhaseTag();
	
	UCustomAbilitySystemComponent* GameStateASC = GetGameStateASC();
	if (!ensure(GameStateASC)) return;

	TArray<FGameplayAbilitySpec*> ActivePhaseSpecs;

	// Find Active Phases and add them to the array
	for (const auto& KVP : ActivePhaseMap)
	{
		const FGameplayAbilitySpecHandle ActiveAbilityHandle = KVP.Key;
		if (FGameplayAbilitySpec* Spec = GameStateASC->FindAbilitySpecFromHandle(ActiveAbilityHandle))
		{
			ActivePhaseSpecs.Emplace(Spec);
		}
	}

	// Remove phase abilities that are not ancestors of the new phase (different phase)
	for (const FGameplayAbilitySpec* ActiveSpec : ActivePhaseSpecs)
	{
		const UCustomGamePhaseAbility* ActivePhaseAbility = CastChecked<UCustomGamePhaseAbility>(ActiveSpec->Ability);
		const FGameplayTag& ActivePhaseTag = ActivePhaseAbility->GetGamePhaseTag();

		// So if the active phase currently matches the incoming phase tag, we allow it.
		// i.e. multiple gameplay abilities can all be associated with the same phase tag.
		// For example,
		// You can be in the, Game.Playing, phase, and then start a sub-phase, like Game.Playing.SuddenDeath
		// Game.Playing phase will still be active, and if someone were to push another one, like,
		// Game.Playing.ActualSuddenDeath, it would end Game.Playing.SuddenDeath phase, but Game.Playing would continue.
		// Similarly if we activated Game.GameOver, all the Game.Playing* phases would end.
		if (!IncomingPhaseTag.MatchesTag(ActivePhaseTag))
		{
			UE_LOG(LogCustom, Log, TEXT("\tEnding Phase '%s' (%s)"), *ActivePhaseTag.ToString(), *GetNameSafe(PhaseAbility));

			FGameplayAbilitySpecHandle HandleToEnd = ActiveSpec->Handle;
			GameStateASC->CancelAbilitiesByFunc([HandleToEnd](const UCustomGameplayAbility* CustomAbility, FGameplayAbilitySpecHandle Handle)
			{
				return Handle == HandleToEnd;
			}, true);
		}
	}

	// Now assign Tag to the entry. This entry should be already valid when StartPhase was called.
	FGamePhaseEntry& Entry = ActivePhaseMap.FindOrAdd(PhaseAbilityHandle);
	Entry.PhaseTag = IncomingPhaseTag;

	// Notify all observers of this phase that it has started.
	NotifyPhaseChanged(IncomingPhaseTag, PhaseStartObservers);
}

void UCustomGamePhaseSubsystem::OnEndPhase(const UCustomGamePhaseAbility* PhaseAbility,
	const FGameplayAbilitySpecHandle PhaseAbilityHandle)
{
	check(PhaseAbility);

	const FGameplayTag& EndedPhaseTag = PhaseAbility->GetGamePhaseTag();
	UE_LOG(LogCustom, Log, TEXT("Ended Phase '%s' (%s)"), *EndedPhaseTag.ToString(), *GetNameSafe(PhaseAbility));

	const FGamePhaseEntry& Entry = ActivePhaseMap.FindChecked(PhaseAbilityHandle);
	Entry.PhaseEndedCallback.ExecuteIfBound(PhaseAbility);

	ActivePhaseMap.Remove(PhaseAbilityHandle);

	// Notify all observers of this phase that it has ended.
	NotifyPhaseChanged(EndedPhaseTag, PhaseEndObservers);
}

UCustomAbilitySystemComponent* UCustomGamePhaseSubsystem::GetGameStateASC() const
{
	return GetWorld()->GetGameState()->FindComponentByClass<UCustomAbilitySystemComponent>();
}

bool UCustomGamePhaseSubsystem::FPhaseObserveInfo::IsMatch(const FGameplayTag& ComparePhaseTag) const
{
	switch(MatchType)
	{
	case EPhaseTagMatchType::ExactMatch:
		return ComparePhaseTag == PhaseTag;
	case EPhaseTagMatchType::PartialMatch:
		return ComparePhaseTag.MatchesTag(PhaseTag);
	}

	return false;
}

void UCustomGamePhaseSubsystem::NotifyPhaseChanged(const FGameplayTag& InPhaseTag, FPhaseObserver& PhaseObserver)
{
	TArray<TWeakObjectPtr<UObject>> NotifiedListeners;
	for (const auto& Pair : PhaseObserver)
	{
		TWeakObjectPtr<UObject> Listener = Pair.Key;
		const FPhaseObserveInfo& ObserveInfo = Pair.Value;
		
		if (ObserveInfo.IsMatch(InPhaseTag))
		{
			ObserveInfo.PhaseCallback.ExecuteIfBound(InPhaseTag);
			NotifiedListeners.Emplace(Listener);
		}
	}

	for (TWeakObjectPtr<UObject> Listener : NotifiedListeners)
	{
		PhaseObserver.Remove(Listener);
	}
}
