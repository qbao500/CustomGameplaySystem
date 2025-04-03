// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CustomPlayerState.h"

#include "Components/LevelExpComponent.h"
#include "Engine/OverlapResult.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "GameFramework/GameStateBase.h"
#include "GAS/Components/CustomAbilitySystemComponent.h"
#include "GAS/Attributes/HealthAttributeSet.h"
#include "Interfaces/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomPlayerState)

ACustomPlayerState::ACustomPlayerState()
{
	// This component's InitAbilityActorInfo is called in CustomCorePawnComponent, which is attached in CustomCharacterBase
	AbilitySystemComponent = CreateDefaultSubobject<UCustomAbilitySystemComponent>("Ability System Component");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	HealthSet = CreateDefaultSubobject<UHealthAttributeSet>("Health Set");

	LevelExpComponent = CreateDefaultSubobject<ULevelExpComponent>("Level Exp Component");
	
	NetUpdateFrequency = 100.0f;
}

UCustomAbilitySystemComponent* ACustomPlayerState::GetCustomAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAbilitySystemComponent* ACustomPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

ULevelExpComponent* ACustomPlayerState::GetLevelExpComponent_Implementation() const
{
	return LevelExpComponent;
}

int32 ACustomPlayerState::GetCurrentLevel_Implementation() const
{
	return LevelExpComponent->GetLevel();
}

int32 ACustomPlayerState::GetCurrentXP_Implementation() const
{
	return LevelExpComponent->GetExp();
}

AActor* ACustomPlayerState::FindNearestPlayer(const FVector& CheckingLocation,
	const TArray<APlayerState*>& PlayerStates, float& OutNearestDistance, const bool bCheckAlive)
{
	AActor* BestActor = nullptr;
	OutNearestDistance = FLT_MAX;
	for (const APlayerState* Player : PlayerStates)
	{
		if (!Player) continue;

		AActor* PlayerActor = Player->GetPawn();
		if (!PlayerActor) continue;

		if (bCheckAlive && UKismetSystemLibrary::DoesImplementInterface(PlayerActor, UCombatInterface::StaticClass()))
		{
			if (!ICombatInterface::Execute_IsAlive(PlayerActor)) continue;
		}
		
		const FVector Loc = PlayerActor->GetActorLocation();
		const float Distance = FVector::Dist(CheckingLocation, Loc);

		if (Distance < OutNearestDistance)
		{
			OutNearestDistance = Distance;
			BestActor = PlayerActor;
		}
	}

	return BestActor;
}

AActor* ACustomPlayerState::FindNearestPlayerFromGameState(const UObject* WorldContextObject,
	const FVector& CheckingLocation, float& OutNearestDistance, const bool bCheckAlive)
{
	const AGameStateBase* GameState = UGameplayStatics::GetGameState(WorldContextObject);
	if (!GameState) return nullptr;

	return FindNearestPlayer(CheckingLocation, GameState->PlayerArray, OutNearestDistance, bCheckAlive);
}

void ACustomPlayerState::GetAliveCharactersWithinRadius(const UObject* WorldContextObject,
	TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& IgnoreActors, const float Radius,
	const FVector& SphereOrigin)
{
	OutOverlappingActors.Empty();
	
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(IgnoreActors);
	
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return;
	
	TArray<FOverlapResult> Overlaps;
	World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
		FCollisionShape::MakeSphere(Radius), SphereParams);
	for (const FOverlapResult& Overlap : Overlaps)
	{
		if (!UKismetSystemLibrary::DoesImplementInterface(Overlap.GetActor(), UCombatInterface::StaticClass())) continue;
		
		if (ICombatInterface::Execute_IsAlive(Overlap.GetActor()))
		{
			OutOverlappingActors.AddUnique(Overlap.GetActor());
		}
	}
}
