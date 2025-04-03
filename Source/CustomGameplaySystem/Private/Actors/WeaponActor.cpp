// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/WeaponActor.h"

#include "DataAssets/FeedbackEffectDataAsset.h"

AWeaponActor::AWeaponActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	SetActorHiddenInGame(true);
	GetWorldTimerManager().SetTimer(EquipDelayTimer, this, &ThisClass::EquipDelayed, SwapDuration, false);
}

void AWeaponActor::UnequipWeapon()
{
	OnUnequipped();

	SetLifeSpan(SwapDuration);
}

const FGameplayTag& AWeaponActor::GetWeaponTag() const
{
	return WeaponTag;
}

void AWeaponActor::SetWeaponTag(const FGameplayTag NewTag)
{
	if (!NewTag.IsValid()) return;

	WeaponTag = NewTag;
}

void AWeaponActor::TriggerMeleeHitEffect(const FHitResult& HitInfo)
{
	if (MeleeHitEffectData)
	{
		MeleeHitEffectData->PlayEffects(this, HitInfo);
	}
}

void AWeaponActor::EquipInstantly()
{
	GetWorldTimerManager().ClearTimer(EquipDelayTimer);
	SetActorHiddenInGame(false);
}

void AWeaponActor::UnequipInstantly()
{
	if (!IsValid(this)) return;
	
	SetLifeSpan(0.0f);
	Destroy();
}

void AWeaponActor::EquipDelayed()
{
	SetActorHiddenInGame(false);

	if (EquipEffectData)
	{
		EquipEffectData->PlayDefaultEffects(this, GetActorLocation(), GetActorRotation());
	}

	OnEquipped();
}
