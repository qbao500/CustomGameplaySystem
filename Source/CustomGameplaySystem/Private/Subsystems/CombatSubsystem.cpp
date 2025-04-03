// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/CombatSubsystem.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "FunctionLibraries/CustomHelperFunctionLibrary.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "Interfaces/CombatInterface.h"
#include "Interfaces/EnemyCombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UCombatSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	CurrentAttackToken = MaxAttackToken;

	Player = UGameplayStatics::GetPlayerCharacter(&InWorld, 0);
}

void UCombatSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Player.Get()) return;
	// Debug
	if (0)
	{
		const FString Token = "Attack token: " + FString::FromInt(CurrentAttackToken) + " | " + FString::FromInt(MaxAttackToken);
		PLFL::PrintString(Token, 2, FColor::Cyan, 84);
		for (auto Enemy : AggressiveEnemies)
		{
			DrawDebugSphere(GetWorld(), Enemy.Key->GetActorLocation(), 20, 16, FColor::Red);
		}
	}

	bool bCanTick = false;
	if (GetWorld()->GetTimeSeconds() - TickRealTime >= TickRate)
	{
		bCanTick = true;
		TickRealTime = GetWorld()->GetTimeSeconds();
	}

	if (!bCanTick) return;

	// Check if should not target player
	if (!ShouldTargetPlayer())
	{
		// Force enemies to stop aggressive
		for (const TTuple<TWeakObjectPtr<AActor>, int32>& EnemyPair : AggressiveEnemies)
		{
			AddAttackToken(EnemyPair.Value);
		}
		AggressiveEnemies.Empty();
		
		return;
	}

	// If not any enemy is attacking, then assign
	if (CurrentAttackToken > 0 && CurrentAttackToken <= MaxAttackToken)
	{
		TArray<TWeakObjectPtr<AActor>> EnemyArray = EnemiesInCombat.Array();
		HFL::SortActorsByDistance(Player->GetActorLocation(), EnemyArray);
		for (TWeakObjectPtr<AActor> Enemy : EnemyArray)
		{
			if (!Enemy.Get()) continue;

			if (AggressiveEnemies.Contains(Enemy)) continue;

			RequestAttack(Enemy.Get());
		}

		return;
	}

	// Check if should re-assign the nearest enemy
	/*AActor* NearestEnemy = GFL::FindNearestActor(Player->GetActorLocation(), EnemiesInCombat);
	if (!NearestEnemy) return;
	
	AActor* EnemyNeedToReplace = nullptr;
	for (const TTuple<TWeakObjectPtr<AActor>, int32>& AttackingEnemy : AggressiveEnemies)
	{
		AActor* Enemy = AttackingEnemy.Key.Get();
		if (!Enemy) continue;

		UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Enemy);
		if (!ASC) continue;
		
		// Check if not actually attacking (with animation), or not stunned
		FGameplayTagContainer Tags;
		Tags.AddTag(GFL::MakeTag("Ability.Attack"));
		Tags.AddTag(GFL::MakeTag("CC.Stun"));
		if (!ASC->GetOwnedGameplayTags().HasAny(Tags))
		{
			// If this enemy is not the nearest enemy, then we replace it with the nearest enemy
			if (NearestEnemy && NearestEnemy != Enemy)
			{
				EnemyNeedToReplace = Enemy;
				break;
			}
		}
	}

	if (EnemyNeedToReplace)
	{
		EndAttack(EnemyNeedToReplace);
		RequestAttack(NearestEnemy, 1);
	}*/
}

TStatId UCombatSubsystem::GetStatId() const
{
	return TStatId();
}

void UCombatSubsystem::RegisterCombat(AActor* EnemyActor)
{
	// Already contain, no need to add again
	if (EnemiesInCombat.Contains(EnemyActor)) return;
	
	if (UKismetSystemLibrary::DoesImplementInterface(EnemyActor, UEnemyCombatInterface::StaticClass()))
	{
		const int32 OldAmount = EnemiesInCombat.Num();
		
		EnemiesInCombat.Emplace(EnemyActor);

		// Event combat start
		if (OldAmount == 0 && EnemiesInCombat.Num() > 0)
		{
			OnCombatStart.Broadcast();
		}

		if (EnemiesInCombat.Num() == 1)
		{
			bMulTiEnemyCombat = false;
		}
		else if (EnemiesInCombat.Num() > 1)
		{
			bMulTiEnemyCombat = true;
		}
		else
		{
			bMulTiEnemyCombat = false;
		}
	}
}

void UCombatSubsystem::DeregisterCombat(AActor* EnemyActor)
{
	if (!EnemiesInCombat.Contains(EnemyActor)) return;
	
	if (UKismetSystemLibrary::DoesImplementInterface(EnemyActor, UEnemyCombatInterface::StaticClass()))
	{
		EnemiesInCombat.Remove(EnemyActor);
	}

	// Make sure to return attack token
	EndAttack(EnemyActor);
}

bool UCombatSubsystem::IsEnemyInCombat(const AActor* EnemyActor) const
{
	return EnemiesInCombat.Contains(EnemyActor);
}

int32 UCombatSubsystem::GetEnemyAmount() const
{
	return EnemiesInCombat.Num();
}

bool UCombatSubsystem::IsMultiEnemyCombatFinished() const
{
	return bMulTiEnemyCombat && GetEnemyAmount() <= 0;
}

bool UCombatSubsystem::RequestAttack(AActor* EnemyActor, const int32 TokenCost)
{
	if (!EnemyActor) return false;

	if (AggressiveEnemies.Contains(EnemyActor)) return false;
	
	if (TokenCost > CurrentAttackToken) return false;

	AddAttackToken(-FMath::Abs(TokenCost));
	AggressiveEnemies.Emplace(EnemyActor, TokenCost);
	
	return true;
}

void UCombatSubsystem::EndAttack(AActor* EnemyActor)
{
	if (const int32 Token = AggressiveEnemies.FindRef(EnemyActor))
	{
		AddAttackToken(Token);
		AggressiveEnemies.Remove(EnemyActor);
	}
}

bool UCombatSubsystem::CheckCanAttack(AActor* EnemyActor) const
{
	if (!EnemyActor) return false;

	return AggressiveEnemies.Contains(EnemyActor);
}

void UCombatSubsystem::StartDangerousAttack(AActor* EnemyActor)
{
	if (UKismetSystemLibrary::DoesImplementInterface(EnemyActor, UEnemyCombatInterface::StaticClass()))
	{
		DangerousEnemies.Emplace(EnemyActor);
	}
}

void UCombatSubsystem::StopDangerousAttack(AActor* EnemyActor)
{
	if (UKismetSystemLibrary::DoesImplementInterface(EnemyActor, UEnemyCombatInterface::StaticClass()))
	{
		DangerousEnemies.Remove(EnemyActor);
	}
}

AActor* UCombatSubsystem::GetMostDangerousEnemy(const FVector& PlayerLocation) const
{
	if (DangerousEnemies.Num() <= 0) return nullptr;

	return HFL::FindNearestActor(PlayerLocation, DangerousEnemies);
}

void UCombatSubsystem::AddAttackToken(const int32 Amount)
{
	CurrentAttackToken += Amount;
	CurrentAttackToken = FMath::Clamp(CurrentAttackToken, 0, MaxAttackToken);
}

bool UCombatSubsystem::ShouldTargetPlayer() const
{
	const ACharacter* PlayerRef = Player.Get();
	if (!PlayerRef) return false;

	if (UKismetSystemLibrary::DoesImplementInterface(PlayerRef, UCombatInterface::StaticClass()))
	{
		return !ICombatInterface::Execute_IsImmuneToDamage(PlayerRef);
	}

	// If not added above interface to the player, then always let the logic run
	return true;
}