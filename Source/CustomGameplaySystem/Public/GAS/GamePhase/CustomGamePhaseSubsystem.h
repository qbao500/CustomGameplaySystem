// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"
#include "CustomGamePhaseSubsystem.generated.h"

class UCustomAbilitySystemComponent;
class UCustomGamePhaseAbility;

// Match rule for message receivers
UENUM(BlueprintType)
enum class EPhaseTagMatchType : uint8
{
	// An exact match will only receive messages with exactly the same channel
	// (e.g., registering for "A.B" will match a broadcast of A.B but not A.B.C)
	ExactMatch,

	// A partial match will receive any messages rooted in the same channel
	// (e.g., registering for "A.B" will match a broadcast of A.B as well as A.B.C)
	PartialMatch
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FGamePhaseDynamicDelegate, const UCustomGamePhaseAbility*, Phase);
DECLARE_DELEGATE_OneParam(FGamePhaseDelegate, const UCustomGamePhaseAbility* Phase);

DECLARE_DYNAMIC_DELEGATE_OneParam(FGamePhaseTagDynamicDelegate, const FGameplayTag&, PhaseTag);
DECLARE_DELEGATE_OneParam(FGamePhaseTagDelegate, const FGameplayTag& PhaseTag);

/**
 * Subsystem for managing Lyra's game phases using gameplay tags in a nested manner, which allows parent and child 
 *		phases to be active at the same time, but not sibling phases.
 * Example: Game.Playing and Game.Playing.WarmUp can coexist, but Game.Playing and Game.ShowingScore cannot. 
 * When a new phase is started, any active phases that are not ancestors will be ended.
 * Example: if Game.Playing and Game.Playing.CaptureTheFlag are active when Game.Playing.PostGame is started, 
 *		Game.Playing will remain active, while Game.Playing.CaptureTheFlag will end.
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UCustomGamePhaseSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	void StartPhase(const TSubclassOf<UCustomGamePhaseAbility>& PhaseAbility, const FGamePhaseDelegate& PhaseEndedCallback = FGamePhaseDelegate());
	
	void WhenPhaseStartsOrIsActive(UObject* Listener, const FGameplayTag& PhaseTag, EPhaseTagMatchType MatchType, const FGamePhaseTagDelegate& WhenPhaseActive);
	void WhenPhaseEnds(UObject* Listener, const FGameplayTag& PhaseTag, EPhaseTagMatchType MatchType, const FGamePhaseTagDelegate& WhenPhaseEnd);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, meta = (DisplayName="Start Phase", AutoCreateRefTerm = "PhaseEnded"))
	void K2_StartPhase(TSubclassOf<UCustomGamePhaseAbility> PhaseAbility, const FGamePhaseDynamicDelegate& PhaseEnded);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, meta = (DisplayName = "When Phase Starts or Is Active", AutoCreateRefTerm = "WhenPhaseActive, PhaseTag", Categories = "GamePhase"))
	void K2_WhenPhaseStartsOrIsActive(UObject* Listener, const FGameplayTag& PhaseTag, EPhaseTagMatchType MatchType, FGamePhaseTagDynamicDelegate WhenPhaseActive);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, meta = (DisplayName = "When Phase Ends", AutoCreateRefTerm = "WhenPhaseEnd, PhaseTag", Categories = "GamePhase"))
	void K2_WhenPhaseEnds(UObject* Listener, const FGameplayTag& PhaseTag, EPhaseTagMatchType MatchType, FGamePhaseTagDynamicDelegate WhenPhaseEnd);
	
	UFUNCTION(BlueprintPure, BlueprintAuthorityOnly, meta = (AutoCreateRefTerm = "PhaseTag", Categories = "GamePhase"))
	bool IsPhaseActive(const FGameplayTag& PhaseTag) const;

protected:

	//~ Begin UWorldSubsystem interface
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;
	//~ End UWorldSubsystem interface

	void OnBeginPhase(const UCustomGamePhaseAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle);
	void OnEndPhase(const UCustomGamePhaseAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle);

private:

	UCustomAbilitySystemComponent* GetGameStateASC() const;

	struct FGamePhaseEntry
	{
		FGameplayTag PhaseTag;
		FGamePhaseDelegate PhaseEndedCallback;
	};

	TMap<FGameplayAbilitySpecHandle, FGamePhaseEntry> ActivePhaseMap;

	struct FPhaseObserveInfo
	{
		bool IsMatch(const FGameplayTag& ComparePhaseTag) const;
	
		FGameplayTag PhaseTag;
		EPhaseTagMatchType MatchType = EPhaseTagMatchType::ExactMatch;
		FGamePhaseTagDelegate PhaseCallback;
	};

	typedef TMap<TWeakObjectPtr<UObject>, FPhaseObserveInfo> FPhaseObserver;
	FPhaseObserver PhaseStartObservers;
	FPhaseObserver PhaseEndObservers;

	void NotifyPhaseChanged(const FGameplayTag& InPhaseTag, FPhaseObserver& PhaseObserver);

	friend class UCustomGamePhaseAbility;
};
