// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "ExpLootDropComponent.generated.h"

class UCustomHealthComponent;
class UExpLootRewardDataAsset;

UENUM()
enum class ELootMode : uint8
{
	// Instantly increase XP and/or give loot.
	Instant,
	// Drop mesh for XP and/or loot.
	Drop,
};

/**
 * This component is used to give/drop XP and/or loot.
 * Then it gives rewards for the Rewarded Actor.
 * Otherwise, you need to call TriggerReward() manually.
 * RewardedActor requires to have a LevelExpComponent and ILevelExpInterface.
 * TODO For now, it only increases XP.
 */
UCLASS(ClassGroup = (Drop), meta = (BlueprintSpawnableComponent))
class CUSTOMGAMEPLAYSYSTEM_API UExpLootDropComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UExpLootDropComponent();
	
	virtual void BeginPlay() override;

	// Give or Drop XP and/or loot, based on Loot Mode.
	// RewardedActor requires to have a LevelExpComponent and ILevelExpInterface.
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_TriggerReward(const AActor* RewardedActor);
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UExpLootRewardDataAsset> ExpLootDataAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ELootMode LootMode = ELootMode::Instant;

	// Require Owner to have CustomHealthComponent
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bAutoRewardWhenOwnerDies = true;

	virtual void RewardXP(const AActor* RewardedActor);
	virtual void RewardLoot(const AActor* RewardedActor);

private:

	UPROPERTY()
	TWeakObjectPtr<UCustomHealthComponent> HealthComponent;

	UFUNCTION()
	void OnDeathStarted(AActor* OwningActor, const AActor* DeathInstigator);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_TriggerReward(const AActor* RewardedActor);

	// Helper
	bool HasAuthority() const;

public:

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
