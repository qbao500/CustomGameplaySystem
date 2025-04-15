// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreGame/CustomGameState.h"

#include "GAS/Components/CustomAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomGameState)

extern ENGINE_API float GAverageFPS;

ACustomGameState::ACustomGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	AbilitySystemComponent = CreateDefaultSubobject<UCustomAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

void ACustomGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACustomGameState, ServerFPS);
}

void ACustomGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ACustomGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (GetLocalRole() == ROLE_Authority)
	{
		ServerFPS = GAverageFPS;
	}
}

UAbilitySystemComponent* ACustomGameState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UCustomAbilitySystemComponent* ACustomGameState::GetCustomAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

float ACustomGameState::GetServerFPS() const
{
	return ServerFPS;
}
