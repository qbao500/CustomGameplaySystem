// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveLoadSystem/SaveLoadSubsystem.h"

#include "CustomGameplayTags.h"
#include "EngineUtils.h"
#include "FunctionLibraries/GameplayTagFunctionLibrary.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "SaveLoadSystem/CustomSaveGame.h"
#include "SaveLoadSystem/SaveLoadInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SaveLoadSubsystem)

USaveLoadSubsystem* USaveLoadSubsystem::Get(const UObject* ContextObject)
{
	return UGameplayStatics::GetGameInstance(ContextObject)->GetSubsystem<USaveLoadSubsystem>();
}

void USaveLoadSubsystem::WriteSaveGame(const FGameplayTag& SaveGameTag, const FSaveGameSignature& OnWrittenDelegate)
{
	UCustomSaveGame* CurrentSaveGame = GetCurrentSaveGame(SaveGameTag, true);

	if (!CurrentSaveGame)
	{
		return;
	}

	WriteSaveGameCallback(SaveGameTag, CurrentSaveGame, OnWrittenDelegate);
}

void USaveLoadSubsystem::LoadSaveGame(const FGameplayTag& SaveGameTag, const FSaveGameSignature& OnLoadedDelegate, const bool bCacheLoadedSave)
{
	const FString SlotName = MakeSlotName(SaveGameTag);
	if (SlotName.IsEmpty()) return;

	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UGameplayStatics::AsyncLoadGameFromSlot(SlotName, 0, 
			FAsyncLoadGameFromSlotDelegate::CreateWeakLambda(this, 
				[this, SaveGameTag, OnLoadedDelegate, bCacheLoadedSave](const FString& SlotName, const int32 UserIndex, USaveGame* LoadedSave)
				{
					OnSaveGameLoaded(LoadedSave, SaveGameTag, SlotName, OnLoadedDelegate, bCacheLoadedSave);
				}));
	}
}

void USaveLoadSubsystem::LoadOrCreateSaveGame(const FGameplayTag& SaveGameTag, const TSubclassOf<UCustomSaveGame> ClassForNewSave,
	const FSaveGameSignature& OnLoadedDelegate, const bool bCacheLoadedSave)
{
	const FString SlotName = MakeSlotName(SaveGameTag);
	if (SlotName.IsEmpty()) return;
	
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UGameplayStatics::AsyncLoadGameFromSlot(SlotName, 0, 
			FAsyncLoadGameFromSlotDelegate::CreateWeakLambda(this, 
				[this, SaveGameTag, OnLoadedDelegate, bCacheLoadedSave](const FString& SlotName, const int32 UserIndex, USaveGame* LoadedSave)
				{
					OnSaveGameLoaded(LoadedSave, SaveGameTag, SlotName, OnLoadedDelegate, bCacheLoadedSave);
				}));
	}
	else
	{
		USaveGame* SaveGame = UGameplayStatics::CreateSaveGameObject(ClassForNewSave);
		OnSaveGameLoaded(SaveGame, SaveGameTag, SlotName, OnLoadedDelegate, bCacheLoadedSave);
	}
}

UCustomSaveGame* USaveLoadSubsystem::CreateSaveGame(const FGameplayTag& SaveGameTag, TSubclassOf<UCustomSaveGame> SaveGameClass)
{
	if (!SaveGameClass || !IsSaveGameTagValid(SaveGameTag)) return nullptr;

	const FString SlotName = MakeSlotName(SaveGameTag);
	if (SlotName.IsEmpty()) return nullptr;

	UCustomSaveGame* SaveGame = Cast<UCustomSaveGame>(UGameplayStatics::CreateSaveGameObject(SaveGameClass));
	if (SaveGame)
	{
		CurrentSaveGameMap.Emplace(SaveGameTag, SaveGame);
	}
	
	return SaveGame;
}

void USaveLoadSubsystem::DeleteSlotSaveGame(const int32 SlotIndex)
{
	// Delete all type of save game with the given SlotIndex
	for (const FGameplayTag& Tag : UGameplayTagsManager::Get().RequestGameplayTagChildren(CustomTags::SaveGame))
	{
		const FString SlotName = MakeSlotNameFromIndex(Tag, SlotIndex);
		if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
		{
			UGameplayStatics::DeleteGameInSlot(SlotName, 0);
		}
	}

	// If the deleted slot is the current slot, then there's no valid slot index
	if (CurrentSlotIndex == SlotIndex)
	{
		SetSlotIndex(INDEX_NONE);
		CurrentSaveGameMap.Empty();
	}
}

bool USaveLoadSubsystem::DoesSaveGameExist(const FGameplayTag& SaveGameTag) const
{
	if (!IsSaveGameTagValid(SaveGameTag)) return false;

	const FString SlotName = MakeSlotName(SaveGameTag);
	if (SlotName.IsEmpty()) return false;

	// Also try to find in the cache
	if (CurrentSaveGameMap.Contains(SaveGameTag))
	{
		return true;
	}

	return UGameplayStatics::DoesSaveGameExist(SlotName, 0);
}

void USaveLoadSubsystem::SetSlotIndex(const int32 NewSlotIndex)
{
	CurrentSlotIndex = NewSlotIndex;
}

int32 USaveLoadSubsystem::GetCurrentSlotIndex() const
{
	return CurrentSlotIndex;
}

UCustomSaveGame* USaveLoadSubsystem::GetCurrentSaveGame(const FGameplayTag& SaveGameTag, const bool bSyncLoadIfNotFound)
{
	UCustomSaveGame* Save = CurrentSaveGameMap.FindRef(SaveGameTag);
	if (!Save && bSyncLoadIfNotFound)
	{
		const FString SlotName = MakeSlotName(SaveGameTag);
		if (SlotName.IsEmpty()) return nullptr;

		if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0)) return nullptr;
		
		Save = Cast<UCustomSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if (Save)
		{
			CurrentSaveGameMap.Emplace(SaveGameTag, Save);
		}
	}

	return Save;
}

FString USaveLoadSubsystem::MakeSlotName(const FGameplayTag& SaveGameTag) const
{
	return MakeSlotNameFromIndex(SaveGameTag, CurrentSlotIndex);
}

FString USaveLoadSubsystem::MakeSlotNameFromIndex(const FGameplayTag& SaveGameTag, const int32 SlotIndex)
{
	if (!SaveGameTag.IsValid() || !SaveGameTag.MatchesTag(CustomTags::SaveGame)) return FString();

	if (SlotIndex < 0)
	{
		PLFL::PrintWarning("USaveLoadSubsystem slot index is invalid, and returning empty SlotName. Please call SetSlotIndex() first.", 1);
		return FString();
	}

	FString TagString = TAG_FL::MakeStringFromGameplayTag(SaveGameTag, "_", 1);
	TagString += FString::Printf(TEXT("_%d"), SlotIndex);

	return TagString;
}

void USaveLoadSubsystem::LoadWorldState(UWorld* InWorld)
{
	if (!InWorld) return;

	if (!DoesSaveGameExist(CustomTags::SaveGame_WorldState)) return;

	CurrentWorld = InWorld;

	FSaveGameSignature Del;
	Del.BindDynamic(this, &ThisClass::LoadWorldStateCallback);
	LoadSaveGame(CustomTags::SaveGame_WorldState, Del);
}

void USaveLoadSubsystem::SaveWorldState(UWorld* InWorld)
{
	if (!InWorld) return;

	CurrentWorld = InWorld;

	FSaveGameSignature Del;
	Del.BindDynamic(this, &ThisClass::SaveWorldStateCallback);
	LoadOrCreateSaveGame(CustomTags::SaveGame_WorldState, UCustomSaveGame::StaticClass(),	Del);
}

void USaveLoadSubsystem::OnSaveGameLoaded(USaveGame* LoadedSave, const FGameplayTag& SaveGameTag,
	const FString& SlotName, const FSaveGameSignature& OnLoadedDelegate, const bool bCacheLoadedSave)
{
	UCustomSaveGame* CustomSaveGame = Cast<UCustomSaveGame>(LoadedSave);
	if (CustomSaveGame && bCacheLoadedSave)
	{
		// Cache the loaded SaveGame object with corresponding Tag
		CurrentSaveGameMap.Emplace(SaveGameTag, CustomSaveGame);
	}
	
	OnLoadedDelegate.ExecuteIfBound(CustomSaveGame, SlotName, CustomSaveGame != nullptr);
}

void USaveLoadSubsystem::LoadWorldStateCallback(UCustomSaveGame* SaveObject, const FString& SlotName,
	const bool bSuccess)
{
	if (!CurrentWorld || !SaveObject) return;

	FString WorldName = CurrentWorld->GetMapName();
	WorldName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	FWorldSaveData* WorldSaveData = SaveObject->WorldSaveDataMap.Find(FName(*WorldName));
	if (!WorldSaveData) return;

	for (FActorIterator It(CurrentWorld); It; ++It)
	{
		AActor* Actor = *It;

		if (!IsValid(Actor) || Actor->IsEditorOnly() || !Actor->Implements<USaveLoadInterface>()) continue;

		const FActorSaveData* ActorSaveData = WorldSaveData->ActorSaveDataMap.Find(Actor->GetFName());
		if (!ActorSaveData) continue;

		// Set the Actor's transform if applicable
		if (ISaveLoadInterface::Execute_ShouldLoadTransform(Actor))
		{
			Actor->SetActorTransform(ActorSaveData->Transform);
		}

		FMemoryReader MemoryReader(ActorSaveData->ByteData);
		FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
		Archive.ArIsSaveGame = true;
		
		// Convert binary array back into actor's variables
		Actor->Serialize(Archive);

		ISaveLoadInterface::Execute_OnActorLoadedFromSave(Actor);
	}
}

void USaveLoadSubsystem::SaveWorldStateCallback(UCustomSaveGame* SaveObject, const FString& SlotName, const bool bSuccess)
{
	if (!CurrentWorld || !SaveObject) return;

	FString WorldName = CurrentWorld->GetMapName();
	WorldName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	TMap<FName, FWorldSaveData>& WorldSaveDataMap = SaveObject->WorldSaveDataMap;
	FWorldSaveData& WorldSaveData = WorldSaveDataMap.FindOrAdd(FName(*WorldName));
	WorldSaveData.WorldName = FName(*WorldName);
	WorldSaveData.ActorSaveDataMap.Empty();

	for (FActorIterator It(CurrentWorld); It; ++It)
	{
		AActor* Actor = *It;

		if (!IsValid(Actor) || Actor->IsEditorOnly() || !Actor->Implements<USaveLoadInterface>()) continue;

		FActorSaveData ActorSaveData;
		ActorSaveData.ActorName = Actor->GetFName();
		ActorSaveData.Transform = Actor->GetActorTransform();

		// Pass the array to fill with data from Actor
		FMemoryWriter MemoryWriter(ActorSaveData.ByteData);
		FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
		// Find only variables with UPROPERTY(SaveGame)
		Archive.ArIsSaveGame = true;
		// Converts Actor's SaveGame UPROPERTY into binary array
		Actor->Serialize(Archive);

		WorldSaveData.ActorSaveDataMap.Emplace(ActorSaveData.ActorName, ActorSaveData);
	}

	WriteSaveGame(CustomTags::SaveGame_WorldState, FSaveGameSignature());
}

void USaveLoadSubsystem::WriteSaveGameCallback(const FGameplayTag& SaveGameTag, UCustomSaveGame* SaveGameObj, const FSaveGameSignature& OnWrittenDelegate)
{
	const FString SlotName = MakeSlotName(SaveGameTag);
	if (SlotName.IsEmpty())
	{
		return;
	}

	UGameplayStatics::AsyncSaveGameToSlot(SaveGameObj, SlotName, 0,
		FAsyncSaveGameToSlotDelegate::CreateWeakLambda(this,
			[this, SaveGameObj, OnWrittenDelegate](const FString& SlotName, const int32 UserIndex, bool bSuccess)
			{
				OnWrittenDelegate.ExecuteIfBound(SaveGameObj, SlotName, bSuccess);
			}));
}

bool USaveLoadSubsystem::IsSaveGameTagValid(const FGameplayTag& SaveGameTag) const
{
	return SaveGameTag.IsValid() && SaveGameTag.MatchesTag(CustomTags::SaveGame);
}
