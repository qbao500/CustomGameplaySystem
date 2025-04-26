// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "CombatSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCombatStart);

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UCombatSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	UFUNCTION(BlueprintCallable)
	void RegisterCombat(AActor* EnemyActor);
	UFUNCTION(BlueprintCallable)
	void DeregisterCombat(AActor* EnemyActor);

	UFUNCTION(BlueprintPure)
	bool IsEnemyInCombat(const AActor* EnemyActor) const;
	UFUNCTION(BlueprintPure)
	int32 GetEnemyAmount() const;
	UFUNCTION(BlueprintPure)
	bool IsMultiEnemyCombatFinished() const;

	UFUNCTION(BlueprintCallable)
	bool RequestAttack(AActor* EnemyActor, const int32 TokenCost = 1);
	UFUNCTION(BlueprintCallable)
	void EndAttack(AActor* EnemyActor);
	UFUNCTION(BlueprintPure)
	bool CheckCanAttack(AActor* EnemyActor) const;

	UFUNCTION(BlueprintCallable)
	void StartDangerousAttack(AActor* EnemyActor);
	UFUNCTION(BlueprintCallable)
	void StopDangerousAttack(AActor* EnemyActor);
	UFUNCTION(BlueprintPure)
	AActor* GetMostDangerousEnemy(const FVector& PlayerLocation) const;

	UPROPERTY(BlueprintAssignable)
	FCombatStart OnCombatStart;

private:

	// Cache AActor, then call Interface function on them.
	// Register Combat makes sure they implemented IEnemyCombatInterface.
	// For example, IEnemyCombatInterface::Execute_FunctionName(EnemyActor, SomeParams);
	TSet<TWeakObjectPtr<AActor>> EnemiesInCombat;

	// Aggressive means the enemy is allowed to go to Player and attack
	TMap<TWeakObjectPtr<AActor>, int32> AggressiveEnemies;
	int32 CurrentAttackToken = 0;
	int32 MaxAttackToken = 2;
	void AddAttackToken(const int32 Amount);

	// Dangerous means the enemy is in attack animation, within anticipation frames.
	// This is defined by AnimNotifyState that put in those animations
	TSet<TWeakObjectPtr<AActor>> DangerousEnemies;

	// Player ref
	TWeakObjectPtr<ACharacter> Player;
	bool ShouldTargetPlayer() const;

	// This defines if player just combat with multiple enemies
	bool bMulTiEnemyCombat = false;

	float TickRealTime = 0.0f;
	const float TickRate = 0.5f;
};