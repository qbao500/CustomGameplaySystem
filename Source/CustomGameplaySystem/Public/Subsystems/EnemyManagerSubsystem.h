// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "EnemyManagerSubsystem.generated.h"

/**
 *  Require enemies to implement IEnemyCombatInterface
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UEnemyManagerSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:

	UEnemyManagerSubsystem();

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	UFUNCTION(BlueprintCallable)
	void RegisterEnemy(AActor* EnemyActor);

	UFUNCTION(BlueprintPure)
	const TSet<AActor*>& GetNearbyEnemies() const;
	UFUNCTION(BlueprintPure)
	TArray<AActor*> GetNearbyEnemiesArray() const;
	UFUNCTION(BlueprintPure)
	bool IsEnemyNearPlayer(const AActor* EnemyActor) const;

private:

	UPROPERTY()
	TSet<AActor*> Enemies;
	UPROPERTY()
	TSet<AActor*> NearbyEnemies;

	TQueue<AActor*> QueueToAdd;

	UPROPERTY()
	TObjectPtr<ACharacter> Player;

	float TickRealTime = 0.0f;
	const float TickRate = 0.5f;

	const float DistanceToConsiderNearPlayer = 3000.0f;
};
