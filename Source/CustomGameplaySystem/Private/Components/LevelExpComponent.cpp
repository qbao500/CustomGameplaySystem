// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LevelExpComponent.h"

#include "CustomGameplayTags.h"
#include "DataAssets/FeedbackEffectDataAsset.h"
#include "DataAssets/LevelUpDataAsset.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/LevelExpInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "SaveLoadSystem/LevelExpSaveGame.h"
#include "SaveLoadSystem/SaveLoadSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LevelExpComponent)

ULevelExpComponent::ULevelExpComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	SetIsReplicatedByDefault(true);
}

void ULevelExpComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(LevelUpData, TEXT("LevelUpData is not set in %s!"), *GetNameSafe(this));

	LoadLevelExp();

	OnChangedLevel.AddDynamic(this, &ULevelExpComponent::OnLevelChanged_Internal);
	OnChangedExp.AddDynamic(this, &ULevelExpComponent::OnExpChanged_Internal);
}

void ULevelExpComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ULevelExpComponent, CurrentLevel);
	DOREPLIFETIME(ULevelExpComponent, CurrentExp);
}

int32 ULevelExpComponent::GetLevel() const
{
	return CurrentLevel;
}

void ULevelExpComponent::AddLevel(const int32 Levels)
{
	const int32 OldLevel = CurrentLevel;

	CurrentLevel += Levels;

	OnChangedLevel.Broadcast(OldLevel, CurrentLevel);
}

int32 ULevelExpComponent::GetExp() const
{
	return CurrentExp;
}

void ULevelExpComponent::AddExp(const int32 Exp)
{
	const int32 OldExp = CurrentExp;

	CurrentExp += Exp;

	OnChangedExp.Broadcast(OldExp, CurrentExp);

	CheckForLevelUp();
}

ULevelExpComponent* ULevelExpComponent::FindLevelExpComponent(const AActor* Actor)
{
	if (!Actor) return nullptr;

	if (UKismetSystemLibrary::DoesImplementInterface(Actor, ULevelExpInterface::StaticClass()))
	{
		return ILevelExpInterface::Execute_GetLevelExpComponent(Actor);
	}

	return Actor->FindComponentByClass<ULevelExpComponent>();
}

void ULevelExpComponent::OnRep_Level(const int32 OldLevel)
{
	OnChangedLevel.Broadcast(OldLevel, CurrentLevel);
}

void ULevelExpComponent::OnRep_Exp(const int32 OldExp)
{
	OnChangedExp.Broadcast(OldExp, CurrentExp);
}

void ULevelExpComponent::CheckForLevelUp()
{
	const int32 PotentialNewLevel = LevelUpData->FindLevelForXP(CurrentExp);
	const int32 PotentialDeltaLevel = PotentialNewLevel - CurrentLevel;
	if (PotentialDeltaLevel > 0)
	{
		for (int32 i = 0; i < PotentialDeltaLevel; ++i)
		{
			AddLevel(1);
		}

		// Play level up effects once only
		if (LevelUpEffectData)
		{
			LevelUpEffectData->PlayDefaultEffects(this, GetAvatar()->GetActorLocation(), FRotator());
		}
	}
}

void ULevelExpComponent::LoadLevelExp()
{
	if (!bUseSaveGame || !SaveGameTag.MatchesTag(CustomTags::SaveGame_LevelExp)) return;

	USaveLoadSubsystem* SaveSystem = USaveLoadSubsystem::Get(this);

	FSaveGameSignature OnLoadedDelegate;
	OnLoadedDelegate.BindDynamic(this, &ULevelExpComponent::OnSaveGameLoaded);
	SaveSystem->LoadOrCreateSaveGame(SaveGameTag, ULevelExpSaveGame::StaticClass(), OnLoadedDelegate);
}

void ULevelExpComponent::OnSaveGameLoaded(UCustomSaveGame* SaveObject, const FString& SlotName, const bool bSuccess)
{
	if (const ULevelExpSaveGame* SaveGame = Cast<ULevelExpSaveGame>(SaveObject))
	{
		CurrentLevel = SaveGame->Level;
		CurrentExp = SaveGame->Experience;

		OnChangedLevel.Broadcast(0, CurrentLevel);
	}
}

void ULevelExpComponent::SaveLevelExp()
{
	if (!bUseSaveGame || !SaveGameTag.MatchesTag(CustomTags::SaveGame_LevelExp)) return;

	USaveLoadSubsystem* SaveSystem = USaveLoadSubsystem::Get(this);
	if (ULevelExpSaveGame* SaveGame = SaveSystem->GetCurrentSaveGame<ULevelExpSaveGame>(SaveGameTag))
	{
		SaveGame->Level = CurrentLevel;
		SaveGame->Experience = CurrentExp;
		
		SaveSystem->WriteSaveGame(SaveGameTag, FSaveGameSignature());
	}
}

void ULevelExpComponent::OnLevelChanged_Internal(const int OldValue, const int NewValue)
{
	if (bAutoSaveWhenLevelChanged)
	{
		SaveLevelExp();
	}
}

void ULevelExpComponent::OnExpChanged_Internal(const int OldValue, const int NewValue)
{
	if (bAutoSaveWhenExpChanged)
	{
		SaveLevelExp();
	}
}

AActor* ULevelExpComponent::GetAvatar() const
{
	check(GetOwner());

	if (GetOwner()->IsA(APawn::StaticClass()))
	{
		return GetOwner();
	}

	if (const APlayerState* PlayerState = Cast<APlayerState>(GetOwner()))
	{
		return PlayerState->GetPawn();
	}

	if (const AController* Controller = Cast<AController>(GetOwner()))
	{
		return Controller->GetPawn();
	}

	return GetOwner();
}
