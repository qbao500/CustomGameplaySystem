// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/EnemyManagerSubsystem.h"

#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "Interfaces/CombatInterface.h"
#include "Interfaces/EnemyCombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

UEnemyManagerSubsystem::UEnemyManagerSubsystem()
{
	
}

void UEnemyManagerSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	Player = UGameplayStatics::GetPlayerCharacter(&InWorld, 0);
}

void UEnemyManagerSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Player) return;

	bool bCanTick = false;
	if (GetWorld()->GetTimeSeconds() - TickRealTime >= TickRate)
	{
		bCanTick = true;
		TickRealTime = GetWorld()->GetTimeSeconds();
	}

	if (!bCanTick) return;

	// Now officially add. This is to prevent changing a container while looping it
	while (!QueueToAdd.IsEmpty())
	{
		AActor* Enemy;
		QueueToAdd.Dequeue(Enemy);

		Enemies.Emplace(Enemy);
	}

	// Remove dead enemies
	TSet<AActor*> DeadEnemies;
	for (AActor* Enemy : Enemies)
	{
		if (!ICombatInterface::Execute_IsAlive(Enemy))
		{
			DeadEnemies.Emplace(Enemy);
		}
	}
	for (const AActor* Enemy : DeadEnemies)
	{
		Enemies.Remove(Enemy);
		NearbyEnemies.Remove(Enemy);
	}

	// Update nearby enemies
	for (AActor* Enemy : Enemies)
	{
		if (FVector::Dist(Enemy->GetActorLocation(), Player->GetActorLocation()) <= DistanceToConsiderNearPlayer)
		{
			if (!NearbyEnemies.Contains(Enemy))
			{
				NearbyEnemies.Emplace(Enemy);
				IEnemyCombatInterface::Execute_OnNearToPlayer(Enemy);
			}
		}
		else
		{
			if (NearbyEnemies.Remove(Enemy))
			{
				IEnemyCombatInterface::Execute_OnAwayFromPlayer(Enemy);
			}
		}
	}
}

TStatId UEnemyManagerSubsystem::GetStatId() const
{
	return TStatId();
}

void UEnemyManagerSubsystem::RegisterEnemy(AActor* EnemyActor)
{
	if (UKismetSystemLibrary::DoesImplementInterface(EnemyActor, UEnemyCombatInterface::StaticClass()))
	{
		// Add to queue first, then will be officially added to Enemies in Tick()
		QueueToAdd.Enqueue(EnemyActor);
	}
}

const TSet<AActor*>& UEnemyManagerSubsystem::GetNearbyEnemies() const
{
	return NearbyEnemies;
}

TArray<AActor*> UEnemyManagerSubsystem::GetNearbyEnemiesArray() const
{
	TArray<AActor*> EnemyArray = NearbyEnemies.Array();
	return EnemyArray;
}

bool UEnemyManagerSubsystem::IsEnemyNearPlayer(const AActor* EnemyActor) const
{
	return NearbyEnemies.Contains(EnemyActor);
}

