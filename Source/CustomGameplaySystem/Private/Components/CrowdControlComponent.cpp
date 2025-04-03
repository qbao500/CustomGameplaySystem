// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CrowdControlComponent.h"

#include "AbilitySystemInterface.h"

UCrowdControlComponent::UCrowdControlComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCrowdControlComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const IAbilitySystemInterface* Interface = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		AbilityComp = Interface->GetAbilitySystemComponent();
	}
}

void UCrowdControlComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
