// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomGameplayTags.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "SaveLoadSystem/CustomSaveGame.h"
#include "AbilityStatusComponent.generated.h"

class UCustomAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FAbilityStatus
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "Ability.Status"))
	FGameplayTag StatusTag = CustomTags::Ability_Status_Locked;
	
	UPROPERTY(BlueprintReadOnly)
	int32 AbilityLevel = 1;
	
	UPROPERTY(BlueprintReadOnly, meta = (Categories = "Input"))
	FGameplayTag InputTag = FGameplayTag();

	FAbilityStatus() {}
	FAbilityStatus(const FGameplayTag& InStatusTag, const int32 InAbilityLevel = 1) : StatusTag(InStatusTag), AbilityLevel(InAbilityLevel) {}
	
	bool operator==(const FGameplayTag& InTag) const;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAbilityStatusChanged, const FGameplayTag&, AbilityTag, const FAbilityStatus&, StatusInfo);

/**
 * Optional component to use with Ability System Component to manage the status of Abilities.
 * Add this component to the same actor that has the Custom Ability System Component.
 * Status in order: Locked -> Eligible -> Unlocked <-> Equipped.
 * TODO This would need to be updated to work with Save Game.
 */
UCLASS(ClassGroup = (AbilitySystem), meta = (BlueprintSpawnableComponent))
class CUSTOMGAMEPLAYSYSTEM_API UAbilityStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UAbilityStatusComponent();
	
	virtual void BeginPlay() override;

	// Should be same Actor that has Custom Ability System Component
	UFUNCTION(BlueprintPure)
	static UAbilityStatusComponent* FindAbilityStatusComponent(const AActor* Actor);

	// If something depends on the status information of abilities, but didn't have the chance to listen to the changes, call this to broadcast all the status.
	UFUNCTION(BlueprintCallable)
	void BroadcastAllAbilityStatus();

	UFUNCTION(BlueprintPure, meta = (AutoCreateRefTerm = "AbilityTag", Categories = "Ability"))
	const FAbilityStatus& GetAbilityStatusInfo(const FGameplayTag& AbilityTag) const;
	UFUNCTION(BlueprintPure, meta = (AutoCreateRefTerm = "AbilityTag", Categories = "Ability"))
	FGameplayTag GetAbilityStatusTag(const FGameplayTag& AbilityTag) const;
	UFUNCTION(BlueprintPure, meta = (AutoCreateRefTerm = "AbilityTag", Categories = "Ability"))
	bool IsAbilityLocked(const FGameplayTag& AbilityTag);
	UFUNCTION(BlueprintPure, meta = (AutoCreateRefTerm = "AbilityTag", Categories = "Ability"))
	bool IsAbilityUnlockedOrEquipped(const FGameplayTag& AbilityTag);
	UFUNCTION(BlueprintPure, meta = (AutoCreateRefTerm = "AbilityTag", Categories = "Ability"))
	int32 GetAbilityLevel(const FGameplayTag& AbilityTag) const;

	// Set ability status to Eligible, only if it's Locked.
	UFUNCTION(BlueprintCallable, Server, Reliable, meta = (AutoCreateRefTerm = "AbilityTag", Categories = "Ability"))
	void Server_AbilityEligibleFromLocked(const FGameplayTag& AbilityTag);

	// Set ability status to Unlocked, only if it's Eligible.
	UFUNCTION(BlueprintCallable, Server, Reliable, meta = (AutoCreateRefTerm = "AbilityTag", Categories = "Ability"))
	void Server_UnlockAbility(const FGameplayTag& AbilityTag);
	
	// Set ability status to Equipped when it's Unlocked. This will also give the ability to the ASC.
	// When it's already Equipped, attempt to switch to another input tag.
	UFUNCTION(BlueprintCallable, Server, Reliable, meta = (AutoCreateRefTerm = "AbilityTag, OptionalInputTag"))
	void Server_EquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& OptionalInputTag = FGameplayTag());

	// Set ability status to Unlocked, only if it's Equipped. This will also remove the ability from the ASC.
	UFUNCTION(BlueprintCallable, Server, Reliable, meta = (AutoCreateRefTerm = "AbilityTag", Categories = "Ability"))
	void Server_UnequipAbility(const FGameplayTag& AbilityTag);

	// Add level to the ability, if it's Unlocked or Equipped.
	UFUNCTION(BlueprintCallable, Server, Reliable, meta = (AutoCreateRefTerm = "AbilityTag", Categories = "Ability"))
	void Server_AddAbilityLevel(const FGameplayTag& AbilityTag, const int32 Amount = 1);
	
	UPROPERTY(BlueprintAssignable)
	FAbilityStatusChanged OnAbilityStatusChanged;

protected:
	
	// Key is the Ability Tag, Value is the a struct containing Status Tag and Ability Level.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Ability"))
	TSet<FGameplayTag> StartingAbilitiesToEquip;

	UPROPERTY(EditDefaultsOnly, Category = "Save Game")
	bool bAutoSaveOnStatusChange = true;
	UPROPERTY(EditDefaultsOnly, Category = "Save Game", meta = (EditCondition = "bAutoSaveOnStatusChange", Categories = "SaveGame"))
	FGameplayTag SaveGameTag = CustomTags::SaveGame_Player_Ability;

	UPROPERTY(BlueprintReadOnly)
	TMap<FGameplayTag, FAbilityStatus> AbilityStatusMap;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCustomAbilitySystemComponent> AbilitySystemComponent;

	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_AbilityEligibleFromLocked(const FGameplayTag& AbilityTag);
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_UnlockAbility(const FGameplayTag& AbilityTag);
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_EquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& OptionalInputTag);
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_UnequipAbility(const FGameplayTag& AbilityTag);
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_AddAbilityLevel(const FGameplayTag& AbilityTag, const int32 Amount = 1);
	
	UFUNCTION()
	virtual void OnPlayerPawnSet(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);
	UFUNCTION()
	virtual void OnAbilitySystemInitialized(UCustomAbilitySystemComponent* ASC);
	UFUNCTION()
	virtual void OnAbilityGiven(const FGameplayAbilitySpec& AbilitySpec);
	UFUNCTION()
	virtual void OnAbilityRemoved(const FGameplayAbilitySpec& AbilitySpec);
	UFUNCTION()
	void OnAbilityInputTagChanged(const FGameplayTag& AbilityTag, const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& NewInputTag);
	
	virtual void GiveStartingAbilities();
	
	void Internal_SetAbilityStatusTag(FAbilityStatus& StatusInfo, const FGameplayTag& StatusTag, const FGameplayTag& AbilityTag) const;
	void Internal_SetAbilityStatusTag(FAbilityStatus* StatusInfo, const FGameplayTag& StatusTag, const FGameplayTag& AbilityTag) const;

private:

	UFUNCTION()
	void Internal_OnAbilityStatusChanged(const FGameplayTag& AbilityTag, const FAbilityStatus& StatusInfo);

	UFUNCTION()
	void OnSavedStatusLoaded(UCustomSaveGame* SaveObject, const FString& SlotName, const bool bSuccess);
	
	// Helpers
	bool HasAuthority() const;
	void GiveAbility(const FGameplayTag& AbilityTag, const int32 Level = 1, const FGameplayTag& OptionalInputTag = FGameplayTag()) const;
	void RemoveAbility(const FGameplayTag& AbilityTag) const;

	FAbilityStatus EmptyStatus = FAbilityStatus();

public:

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
