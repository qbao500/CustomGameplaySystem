// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "SaveLoadSystem/CustomSaveGame.h"
#include "LevelExpComponent.generated.h"

class UFeedbackEffectDataAsset;
class ULevelUpDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStatChanged, const int32, OldValue, const int32, NewValue);

UCLASS(ClassGroup = (Level), meta = (BlueprintSpawnableComponent))
class CUSTOMGAMEPLAYSYSTEM_API ULevelExpComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	ULevelExpComponent();

	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintPure)
	int32 GetLevel() const;
	UFUNCTION(BlueprintCallable)
	void AddLevel(const int32 Levels);
	
	UFUNCTION(BlueprintPure)
	int32 GetExp() const;
	UFUNCTION(BlueprintCallable)
	void AddExp(const int32 Exp);
	
	UPROPERTY(BlueprintAssignable)
	FStatChanged OnChangedLevel;
	UPROPERTY(BlueprintAssignable)
	FStatChanged OnChangedExp;

	UFUNCTION(BlueprintPure, Category = "Level Exp")
	static ULevelExpComponent* FindLevelExpComponent(const AActor* Actor);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULevelUpDataAsset> LevelUpData;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFeedbackEffectDataAsset> LevelUpEffectData;

	UPROPERTY(EditDefaultsOnly, Category = "Save Game")
	bool bUseSaveGame = false;
	UPROPERTY(EditDefaultsOnly, Category = "Save Game", meta = (EditCondition = "bUseSaveGame", Categories = "SaveGame.LevelExp"))
	FGameplayTag SaveGameTag = FGameplayTag::RequestGameplayTag("SaveGame.LevelExp");
	UPROPERTY(EditDefaultsOnly, Category = "Save Game", meta = (EditCondition = "bUseSaveGame"))
	bool bAutoSaveWhenLevelChanged = true;
	UPROPERTY(EditDefaultsOnly, Category = "Save Game", meta = (EditCondition = "bUseSaveGame"))
	bool bAutoSaveWhenExpChanged = false;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 CurrentLevel = 1;
	UFUNCTION()
	virtual void OnRep_Level(int32 OldLevel);

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Exp)
	int32 CurrentExp = 0;
	UFUNCTION()
	virtual void OnRep_Exp(int32 OldExp);

private:

	void CheckForLevelUp();

	void LoadLevelExp();
	UFUNCTION()
	void OnSaveGameLoaded(UCustomSaveGame* SaveObject, const FString& SlotName, const bool bSuccess);
	
	void SaveLevelExp();

	UFUNCTION()
	void OnLevelChanged_Internal(const int OldValue, const int NewValue);
	UFUNCTION()
	void OnExpChanged_Internal(const int OldValue, const int NewValue);

	AActor* GetAvatar() const;

public:

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
