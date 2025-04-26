// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveLoadSubsystem.generated.h"

class USaveGame;
class UCustomSaveGame;

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FSaveGameSignature, UCustomSaveGame*, SaveObject, const FString&, SlotName, const bool, bSuccess);

/**
 * Make sure to call SetSlotIndex() before calling any functions. Valid SlotIndex is >= 0.
 * TODO: Upgrade to use UserIndex
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API USaveLoadSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	static USaveLoadSubsystem* Get(const UObject* ContextObject);
	
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "SaveGameTag, OnWrittenDelegate", Categories = "SaveGame"))
	void WriteSaveGame(const FGameplayTag& SaveGameTag, const FSaveGameSignature& OnWrittenDelegate);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "SaveGameTag, OnLoadedDelegate", Categories = "SaveGame"))
	void LoadSaveGame(const FGameplayTag& SaveGameTag, const FSaveGameSignature& OnLoadedDelegate, const bool bCacheLoadedSave = true);
	
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "SaveGameTag, OnLoadedDelegate", Categories = "SaveGame"))
	void LoadOrCreateSaveGame(const FGameplayTag& SaveGameTag, TSubclassOf<UCustomSaveGame> ClassForNewSave,
		const FSaveGameSignature& OnLoadedDelegate, const bool bCacheLoadedSave = true);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "SaveGameTag", Categories = "SaveGame"))
	UCustomSaveGame* CreateSaveGame(const FGameplayTag& SaveGameTag, TSubclassOf<UCustomSaveGame> SaveGameClass);
	template<class T>
	T* CreateSaveGame(const FGameplayTag& SaveGameTag, TSubclassOf<UCustomSaveGame> SaveGameClass)
	{
		return Cast<T>(CreateSaveGame(SaveGameTag, SaveGameClass));
	}
	
	UFUNCTION(BlueprintCallable)
	void DeleteSlotSaveGame(const int32 SlotIndex);

	UFUNCTION(BlueprintPure, meta = (AutoCreateRefTerm = "SaveGameTag", Categories = "SaveGame"))
	bool DoesSaveGameExist(const FGameplayTag& SaveGameTag) const;
	
	UFUNCTION(BlueprintCallable)
	void SetSlotIndex(int32 NewSlotIndex);
	
	UFUNCTION(BlueprintPure)
	int32 GetCurrentSlotIndex() const;
	
	UFUNCTION(BlueprintPure, meta = (AutoCreateRefTerm = "SaveGameTag", Categories = "SaveGame"))
	UCustomSaveGame* GetCurrentSaveGame(const FGameplayTag& SaveGameTag, const bool bSyncLoadIfNotFound = true);
	template<class T>
	T* GetCurrentSaveGame(const FGameplayTag& SaveGameTag, const bool bSyncLoadIfNotFound = true)
	{
		return Cast<T>(GetCurrentSaveGame(SaveGameTag, bSyncLoadIfNotFound));
	}

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "SaveGameTag", Categories = "SaveGame"))
	FString MakeSlotName(const FGameplayTag& SaveGameTag) const;

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "SaveGameTag", Categories = "SaveGame"))
	static FString MakeSlotNameFromIndex(const FGameplayTag& SaveGameTag, const int32 SlotIndex);

	// Should load World in AGameModeBase::Init()
	void LoadWorldState(UWorld* InWorld);
	void SaveWorldState(UWorld* InWorld);

private:

	int32 CurrentSlotIndex = INDEX_NONE;

	UPROPERTY(Transient)
	TMap<FGameplayTag, UCustomSaveGame*> CurrentSaveGameMap;

	UPROPERTY(Transient)
	TObjectPtr<UWorld> CurrentWorld;

	void OnSaveGameLoaded(USaveGame* LoadedSave, const FGameplayTag& SaveGameTag, const FString& SlotName,
		const FSaveGameSignature& OnLoadedDelegate, const bool bCacheLoadedSave);

	UFUNCTION()
	void LoadWorldStateCallback(UCustomSaveGame* SaveObject, const FString& SlotName, const bool bSuccess);
	UFUNCTION()
	void SaveWorldStateCallback(UCustomSaveGame* SaveObject, const FString& SlotName, const bool bSuccess);
	
	void WriteSaveGameCallback(const FGameplayTag& SaveGameTag, UCustomSaveGame* SaveGameObj,
		const FSaveGameSignature& OnWrittenDelegate);

	bool IsSaveGameTagValid(const FGameplayTag& SaveGameTag) const;
};