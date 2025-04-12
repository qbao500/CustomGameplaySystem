// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Components/AbilityStatusComponent.h"

#include "CustomGameplayTags.h"
#include "Character/CustomCorePawnComponent.h"
#include "GAS/CustomAbilitySystemLibrary.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "GameFramework/PlayerState.h"
#include "GAS/Components/CustomAbilitySystemComponent.h"
#include "SaveLoadSystem/AbilityStatusSaveGame.h"
#include "SaveLoadSystem/SaveLoadSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AbilityStatusComponent)

bool FAbilityStatus::operator==(const FGameplayTag& InTag) const
{
	return StatusTag == InTag;
}

UAbilityStatusComponent::UAbilityStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	SetIsReplicatedByDefault(true);
}

void UAbilityStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	// If this is added to Player State (most likely), the Pawn of PlayerState at this point can be null (on Client side)
	if (APlayerState* PlayerState = Cast<APlayerState>(GetOwner()))
	{
		if (!PlayerState->GetPawn())
		{
			// No Pawn (on Client), so we need to wait until the Pawn is set on PlayerState
			PlayerState->OnPawnSet.AddDynamic(this, &ThisClass::OnPlayerPawnSet);
		}
		else
		{
			// Pawn is already set (on Server or Standalone), so we can call this directly
			OnPlayerPawnSet(PlayerState, PlayerState->GetPawn(), nullptr);
		}
	}
	else if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		OnPlayerPawnSet(Pawn->GetPlayerState(), Pawn, nullptr);
	}
}

UAbilityStatusComponent* UAbilityStatusComponent::FindAbilityStatusComponent(const AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<UAbilityStatusComponent>() : nullptr;
}

void UAbilityStatusComponent::BroadcastAllAbilityStatus()
{
	for (const auto& Pair : AbilityStatusMap)
	{
		const FGameplayTag& AbilityTag = Pair.Key;
		const FAbilityStatus& Status = Pair.Value;
		OnAbilityStatusChanged.Broadcast(AbilityTag, Status);
	}
}

const FAbilityStatus& UAbilityStatusComponent::GetAbilityStatusInfo(const FGameplayTag& AbilityTag) const
{
	if (const FAbilityStatus* Info = AbilityStatusMap.Find(AbilityTag))
	{
		return *Info;
	}

	return EmptyStatus;
}

FGameplayTag UAbilityStatusComponent::GetAbilityStatusTag(const FGameplayTag& AbilityTag) const
{
	const FAbilityStatus* CurrentStatus = AbilityStatusMap.Find(AbilityTag);
	return CurrentStatus ? CurrentStatus->StatusTag : CustomTags::Ability_Status_Locked;
}

bool UAbilityStatusComponent::IsAbilityLocked(const FGameplayTag& AbilityTag)
{
	const FAbilityStatus* CurrentStatus = AbilityStatusMap.Find(AbilityTag);
	return CurrentStatus ? CurrentStatus->StatusTag == CustomTags::Ability_Status_Locked : true;
}

bool UAbilityStatusComponent::IsAbilityUnlockedOrEquipped(const FGameplayTag& AbilityTag)
{
	if (const FAbilityStatus* CurrentStatus = AbilityStatusMap.Find(AbilityTag))
	{
		return CurrentStatus->StatusTag == CustomTags::Ability_Status_Unlocked || CurrentStatus->StatusTag == CustomTags::Ability_Status_Equipped;
	}

	return false;
}

int32 UAbilityStatusComponent::GetAbilityLevel(const FGameplayTag& AbilityTag) const
{
	const FAbilityStatus* CurrentStatus = AbilityStatusMap.Find(AbilityTag);
	return CurrentStatus ? CurrentStatus->AbilityLevel : 0;
}

void UAbilityStatusComponent::Server_AbilityEligibleFromLocked_Implementation(const FGameplayTag& AbilityTag)
{
	Multicast_AbilityEligibleFromLocked(AbilityTag);
}

void UAbilityStatusComponent::Server_UnlockAbility_Implementation(const FGameplayTag& AbilityTag)
{
	const FAbilityStatus* CurrentStatus = AbilityStatusMap.Find(AbilityTag);
	if (!CurrentStatus) return;

	if (*CurrentStatus != CustomTags::Ability_Status_Eligible) return;

	Multicast_UnlockAbility(AbilityTag);
}

void UAbilityStatusComponent::Server_EquipAbility_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& OptionalInputTag)
{
	const FAbilityStatus* CurrentStatus = AbilityStatusMap.Find(AbilityTag);
	if (!CurrentStatus) return;

	if (*CurrentStatus == CustomTags::Ability_Status_Unlocked)
	{
		Multicast_EquipAbility(AbilityTag, OptionalInputTag);
	}
	else if (*CurrentStatus == CustomTags::Ability_Status_Equipped)
	{
		// If the ability is already equipped, then try to switch to another input tag
		if (!OptionalInputTag.IsValid()) return;

		check(AbilitySystemComponent);
		const FGameplayAbilitySpec* AbilitySpec = AFL::GetAbilitySpecFromTag(AbilitySystemComponent, AbilityTag);
		if (!AbilitySpec) return;
		
		const FGameplayTag& CurrentInput = AFL::GetInputTagFromSpec(*AbilitySpec);

		// If being equipped with the same input tag, then do nothing
		if (CurrentInput == OptionalInputTag) return;

		// If passed everything, then re-equip the ability with the new input tag
		Multicast_EquipAbility(AbilityTag, OptionalInputTag);
	}
}

void UAbilityStatusComponent::Server_UnequipAbility_Implementation(const FGameplayTag& AbilityTag)
{
	const FAbilityStatus* CurrentStatus = AbilityStatusMap.Find(AbilityTag);
	if (!CurrentStatus) return;

	if (*CurrentStatus != CustomTags::Ability_Status_Equipped) return;

	Multicast_UnequipAbility(AbilityTag);
}

void UAbilityStatusComponent::Server_AddAbilityLevel_Implementation(const FGameplayTag& AbilityTag, const int32 Amount)
{
	const FAbilityStatus* CurrentStatus = AbilityStatusMap.Find(AbilityTag);
	if (!CurrentStatus) return;

	if (*CurrentStatus != CustomTags::Ability_Status_Equipped && *CurrentStatus != CustomTags::Ability_Status_Unlocked) return;
	
	Multicast_AddAbilityLevel(AbilityTag, Amount);
}

void UAbilityStatusComponent::Multicast_AbilityEligibleFromLocked_Implementation(const FGameplayTag& AbilityTag)
{
	FAbilityStatus& StatusInfo = AbilityStatusMap.FindOrAdd(AbilityTag);
	if (!StatusInfo.StatusTag.IsValid())
	{
		StatusInfo.StatusTag = CustomTags::Ability_Status_Locked;
	}
	
	if (StatusInfo == CustomTags::Ability_Status_Locked)
	{
		Internal_SetAbilityStatusTag(StatusInfo, CustomTags::Ability_Status_Eligible, AbilityTag);
	}
}

void UAbilityStatusComponent::Multicast_UnlockAbility_Implementation(const FGameplayTag& AbilityTag)
{
	FAbilityStatus* StatusInfo = AbilityStatusMap.Find(AbilityTag);
	
	// This should always valid, since it's already passed the check on Server
	check(StatusInfo);

	Internal_SetAbilityStatusTag(StatusInfo, CustomTags::Ability_Status_Unlocked, AbilityTag);
}

void UAbilityStatusComponent::Multicast_EquipAbility_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& OptionalInputTag)
{
	if (const FAbilityStatus* StatusInfo = AbilityStatusMap.Find(AbilityTag))
	{
		GiveAbility(AbilityTag, StatusInfo->AbilityLevel, OptionalInputTag);
	}
}

void UAbilityStatusComponent::Multicast_UnequipAbility_Implementation(const FGameplayTag& AbilityTag)
{
	RemoveAbility(AbilityTag);
}

void UAbilityStatusComponent::Multicast_AddAbilityLevel_Implementation(const FGameplayTag& AbilityTag,	const int32 Amount)
{
	FAbilityStatus* CurrentStatus = AbilityStatusMap.Find(AbilityTag);
	
	// This should always valid, since it's already passed the check on Server
	check(CurrentStatus);

	CurrentStatus->AbilityLevel += Amount;

	// Broadcast the change
	OnAbilityStatusChanged.Broadcast(AbilityTag, *CurrentStatus);

	// If the ability is in ASC (Equipped), then upgrade it too
	if (CurrentStatus->StatusTag == CustomTags::Ability_Status_Equipped)
	{
		AbilitySystemComponent->UpgradeAbilityLevel(AbilityTag, EAbilityUpgradeMethod::Default, 1);
	}
}

void UAbilityStatusComponent::OnPlayerPawnSet(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	UCustomCorePawnComponent* CorePawnComponent = UCustomCorePawnComponent::FindCorePawnComponent(NewPawn);
	if (!CorePawnComponent) return;

	FAbilityComponentInitialized::FDelegate Delegate;
	Delegate.BindDynamic(this, &ThisClass::OnAbilitySystemInitialized);
	CorePawnComponent->OnAbilitySystemInitialized_RegisterAndCall(Delegate);
}

void UAbilityStatusComponent::OnAbilitySystemInitialized(UCustomAbilitySystemComponent* ASC)
{
	check(ASC);
	
	AbilitySystemComponent = ASC;

	FAbilityGiven Delegate;
	Delegate.BindDynamic(this, &ThisClass::OnAbilityGiven);
	AbilitySystemComponent->ListenToAbilityGivenEvent(Delegate);

	AbilitySystemComponent->OnAbilityRemoved.AddDynamic(this, &ThisClass::OnAbilityRemoved);

	AbilitySystemComponent->OnAbilityInputTagChanged.AddDynamic(this, &ThisClass::OnAbilityInputTagChanged);

	OnAbilityStatusChanged.AddDynamic(this, &ThisClass::Internal_OnAbilityStatusChanged);
	
	GiveStartingAbilities();
}

void UAbilityStatusComponent::OnAbilityGiven(const FGameplayAbilitySpec& AbilitySpec)
{
	const FGameplayTag AbilityTag = UCustomAbilitySystemLibrary::GetFirstAbilityTagFromSpec(AbilitySpec);
	if (AbilityTag.IsValid())
	{
		FAbilityStatus& Info = AbilityStatusMap.Emplace(AbilityTag);
		Info.AbilityLevel = AbilitySpec.Level;
		Info.InputTag = UCustomAbilitySystemLibrary::GetInputTagFromSpec(AbilitySpec);
		Internal_SetAbilityStatusTag(Info, CustomTags::Ability_Status_Equipped, AbilityTag);
	}
}

void UAbilityStatusComponent::OnAbilityRemoved(const FGameplayAbilitySpec& AbilitySpec)
{
	const FGameplayTag& AbilityTag = AFL::GetFirstAbilityTagFromSpec(AbilitySpec);
	if (!AbilityTag.IsValid()) return;

	if (FAbilityStatus* Info = AbilityStatusMap.Find(AbilityTag))
	{
		// Only set back to Unlocked if it was Equipped
		if (*Info == CustomTags::Ability_Status_Equipped)
		{
			Info->InputTag = FGameplayTag::EmptyTag;
			Internal_SetAbilityStatusTag(Info, CustomTags::Ability_Status_Unlocked, AbilityTag);
		}
	}
}

void UAbilityStatusComponent::OnAbilityInputTagChanged(const FGameplayTag& AbilityTag,
	const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& NewInputTag)
{
	if (!AbilityTag.IsValid()) return;

	FAbilityStatus& Info = AbilityStatusMap.FindOrAdd(AbilityTag);
	Info.StatusTag = CustomTags::Ability_Status_Equipped;		// Only abilities added to ASC can trigger this event, which mean it should be always Equipped
	Info.AbilityLevel = AbilitySpec.Level;
	Info.InputTag = NewInputTag;

	OnAbilityStatusChanged.Broadcast(AbilityTag, Info);
}

void UAbilityStatusComponent::GiveStartingAbilities()
{
	USaveLoadSubsystem* SaveSystem = USaveLoadSubsystem::Get(this);
	if (SaveSystem->DoesSaveGameExist(SaveGameTag))
	{
		FSaveGameSignature OnLoadedDelegate;
		OnLoadedDelegate.BindDynamic(this, &ThisClass::OnSavedStatusLoaded);
		SaveSystem->LoadSaveGame(SaveGameTag, OnLoadedDelegate);
	}
	else
	{
		// Make a new save game so the subsystem can cache it
		SaveSystem->CreateSaveGame(SaveGameTag, UAbilityStatusSaveGame::StaticClass());
		
		for (const FGameplayTag& AbilityTag : StartingAbilitiesToEquip)
		{
			GiveAbility(AbilityTag);
		}
	}
}

void UAbilityStatusComponent::Internal_SetAbilityStatusTag(FAbilityStatus& StatusInfo, const FGameplayTag& StatusTag, const FGameplayTag& AbilityTag) const
{
	StatusInfo.StatusTag = StatusTag;

	OnAbilityStatusChanged.Broadcast(AbilityTag, StatusInfo);
}

void UAbilityStatusComponent::Internal_SetAbilityStatusTag(FAbilityStatus* StatusInfo, const FGameplayTag& StatusTag, const FGameplayTag& AbilityTag) const
{
	if (!StatusInfo) return;

	StatusInfo->StatusTag = StatusTag;

	OnAbilityStatusChanged.Broadcast(AbilityTag, *StatusInfo);
}

void UAbilityStatusComponent::Internal_OnAbilityStatusChanged(const FGameplayTag& AbilityTag,
	const FAbilityStatus& StatusInfo)
{
	if (!bAutoSaveOnStatusChange) return;
	
	USaveLoadSubsystem* SaveSystem = USaveLoadSubsystem::Get(this);
	
	USaveGame* SaveGame = SaveSystem->GetCurrentSaveGame(SaveGameTag);
	if (UAbilityStatusSaveGame* AbilitySaveGame = Cast<UAbilityStatusSaveGame>(SaveGame))
	{
		AbilitySaveGame->SetAbilityStatusMap(AbilityStatusMap);
		SaveSystem->WriteSaveGame(SaveGameTag, FSaveGameSignature());
	}
}

void UAbilityStatusComponent::OnSavedStatusLoaded(UCustomSaveGame* SaveObject, const FString& SlotName, const bool bSuccess)
{
	if (const UAbilityStatusSaveGame* AbilitySave = Cast<UAbilityStatusSaveGame>(SaveObject))
	{
		AbilityStatusMap = AbilitySave->GetAbilityStatusMap();

		BroadcastAllAbilityStatus();

		for (const TPair<FGameplayTag, FAbilityStatus>& Pair : AbilityStatusMap)
		{
			const FGameplayTag& AbilityTag = Pair.Key;
			const FAbilityStatus& StatusInfo = Pair.Value;

			if (StatusInfo == CustomTags::Ability_Status_Equipped)
			{
				GiveAbility(AbilityTag, StatusInfo.AbilityLevel, StatusInfo.InputTag);
			}
		}
	}
}

bool UAbilityStatusComponent::HasAuthority() const
{
	check(GetOwner())
	return GetOwner()->HasAuthority();
}

void UAbilityStatusComponent::GiveAbility(const FGameplayTag& AbilityTag, const int32 Level, const FGameplayTag& OptionalInputTag) const
{
	if (!HasAuthority() || !AbilityTag.IsValid()) return;

	check(AbilitySystemComponent);

	// After the ability is actually given, the OnAbilityGiven will be called, which will set the status to Equipped
	// This guarantees that the ability information in ASC is up-to-date when OnAbilityStatusChanged is called
	AbilitySystemComponent->GiveAbilityByTag(AbilityTag, Level, OptionalInputTag);
}

void UAbilityStatusComponent::RemoveAbility(const FGameplayTag& AbilityTag) const
{
	if (!HasAuthority() || !AbilityTag.IsValid()) return;

	check(AbilitySystemComponent);

	// After the ability is actually removed, the OnAbilityRemoved will be called, which will set the status to Unlocked
	AbilitySystemComponent->RemoveAbilityByTag(AbilityTag);
}

#if WITH_EDITOR
void UAbilityStatusComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif