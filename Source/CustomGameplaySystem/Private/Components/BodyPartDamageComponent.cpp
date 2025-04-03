// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/BodyPartDamageComponent.h"

UBodyPartDamageComponent::UBodyPartDamageComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UBodyPartDamageComponent::CalculateBodyPartDamage(const FHitResult& HitInfo, float& DamageRef) const
{
	const UPhysicalMaterial* Mat = HitInfo.PhysMaterial.Get();
	if (!Mat) return;

	if (const float Multiplier = DamageMultiplier.FindRef(Mat->SurfaceType))
	{
		DamageRef *= Multiplier;
	}
}
