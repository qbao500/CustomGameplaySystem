// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CustomCharacterWithAbilities.h"
#include "GAS/Components/CustomAbilitySystemComponent.h"
#include "GAS/Attributes/HealthAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomCharacterWithAbilities)

ACustomCharacterWithAbilities::ACustomCharacterWithAbilities()
{
	AbilitySystemComponent = CreateDefaultSubobject<UCustomAbilitySystemComponent>("Ability System Component");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	HealthSet = CreateDefaultSubobject<UHealthAttributeSet>("HealthSet");
}

void ACustomCharacterWithAbilities::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void ACustomCharacterWithAbilities::BeginPlay()
{
	Super::BeginPlay();

	OnAbilitySystemInitialized(AbilitySystemComponent);
}

UAbilitySystemComponent* ACustomCharacterWithAbilities::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
