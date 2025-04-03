// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CharacterSpeedComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

UCharacterSpeedComponent::UCharacterSpeedComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UCharacterSpeedComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!GetOwner()) return;

	CharacterMoveComp = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();
	if (!CharacterMoveComp) return;

	SetSpeedMode(DefaultSpeedMode);
}

void UCharacterSpeedComponent::SetSpeedMode(const ESpeedMode NewSpeedMode)
{
	if (!CharacterMoveComp) return;
	
	CurrentSpeedMode = NewSpeedMode;

	switch (CurrentSpeedMode)
	{
	case ESpeedMode::Walking:
		CharacterMoveComp->MaxWalkSpeed = WalkSpeed;
		break;
	case ESpeedMode::Jogging:
		CharacterMoveComp->MaxWalkSpeed = JogSpeed;
		break;
	case ESpeedMode::Sprinting:
		CharacterMoveComp->MaxWalkSpeed = SprintSpeed;
		break;
	default: ;
	}
}


