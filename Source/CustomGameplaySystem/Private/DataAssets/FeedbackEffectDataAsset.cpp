// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/FeedbackEffectDataAsset.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

const FFeedbackEffect& UFeedbackEffectDataAsset::GetEffectFromPhysMat(const UPhysicalMaterial* PhysicalMaterial) const
{
	if (!PhysicalMaterial) return DefaultEffect;

	if (const FFeedbackEffect* Effect = SurfaceEffects.Find(PhysicalMaterial->SurfaceType))
	{
		return *Effect;
	}

	return DefaultEffect;
}

const FFeedbackEffect& UFeedbackEffectDataAsset::GetEffectFromSurface(const TEnumAsByte<EPhysicalSurface> PhysicalSurface) const
{
	if (const FFeedbackEffect* Effect = SurfaceEffects.Find(PhysicalSurface))
	{
		return *Effect;
	}

	return DefaultEffect;
}

const FFeedbackEffect& UFeedbackEffectDataAsset::GetDefaultEffect() const
{
	return DefaultEffect;
}

USoundBase* UFeedbackEffectDataAsset::GetDefaultRandomSound() const
{
	if (DefaultEffect.Sfx.Num() <= 0) return nullptr;

	return DefaultEffect.Sfx[FMath::RandRange(0, DefaultEffect.Sfx.Num() - 1)];
}

UNiagaraSystem* UFeedbackEffectDataAsset::GetDefaultRandomNiagara() const
{
	if (DefaultEffect.Vfx.Num() <= 0) return nullptr;

	return DefaultEffect.Vfx[FMath::RandRange(0, DefaultEffect.Vfx.Num() - 1)];
}

USoundAttenuation* UFeedbackEffectDataAsset::GetSoundAttenuation() const
{
	return SoundAttenuation;
}

void UFeedbackEffectDataAsset::PlayVFX(const UObject* WorldContextObject, const FHitResult& HitInfo)
{
	const FFeedbackEffect& Effect = GetEffectFromPhysMat(HitInfo.PhysMaterial.Get());

	const FVector& Loc = HitInfo.ImpactPoint;
	const FRotator Rot = HitInfo.ImpactNormal.Cross(FVector::UpVector).Rotation();

	PlayVFX_Internal(WorldContextObject, Effect, Loc, Rot);
}

void UFeedbackEffectDataAsset::PlaySFX(const UObject* WorldContextObject, const FHitResult& HitInfo)
{
	const FFeedbackEffect& Effect = GetEffectFromPhysMat(HitInfo.PhysMaterial.Get());
	
	const FVector& Loc = HitInfo.ImpactPoint;
	const FRotator Rot = HitInfo.ImpactNormal.Cross(FVector::UpVector).Rotation();

	PlaySFX_Internal(WorldContextObject, Effect, Loc, Rot);
}

void UFeedbackEffectDataAsset::PlayEffects(UObject* WorldContextObject, const FHitResult& HitInfo)
{
	PlaySFX(WorldContextObject, HitInfo);
	PlayVFX(WorldContextObject, HitInfo);
}

void UFeedbackEffectDataAsset::PlayDefaultEffects(UObject* WorldContextObject, const FVector Location, const FRotator Rotation)
{
	PlaySFX_Internal(WorldContextObject, DefaultEffect, Location, Rotation);
	PlayVFX_Internal(WorldContextObject, DefaultEffect, Location, Rotation);
}

void UFeedbackEffectDataAsset::PlayVFX_Internal(const UObject* WorldContext, const FFeedbackEffect& Effect, const FVector& Loc, const FRotator& Rot)
{
	if (Effect.Vfx.Num() <= 0) return;
	
	if (Effect.bRandomizeVfx)
	{
		UNiagaraSystem* Niagara = Effect.Vfx[FMath::RandRange(0, Effect.Vfx.Num() - 1)];
		if (!Niagara) return;

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContext, Niagara, Loc, Rot, FVector(1), true,
			true, ENCPoolMethod::AutoRelease);
	}
	else
	{
		for (UNiagaraSystem* Niagara : Effect.Vfx)
		{
			if (!Niagara) continue;

			UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContext, Niagara, Loc, Rot, FVector(1), true,
				true, ENCPoolMethod::AutoRelease);
		}
	}
}

void UFeedbackEffectDataAsset::PlaySFX_Internal(const UObject* WorldContext, const FFeedbackEffect& Effect, const FVector& Loc, const FRotator& Rot)
{
	if (Effect.Sfx.Num() <= 0) return;
	
	if (Effect.bRandomizeSfx)
	{
		USoundBase* Sound = Effect.Sfx[FMath::RandRange(0, Effect.Sfx.Num() - 1)];
		if (!Sound) return;

		UGameplayStatics::PlaySoundAtLocation(WorldContext, Sound, Loc, Rot, 1.0f, 1.0f, 0.0f, SoundAttenuation);
	}
	else
	{
		for (USoundBase* Sound : Effect.Sfx)
		{
			if (!Sound) return;

			UGameplayStatics::PlaySoundAtLocation(WorldContext, Sound, Loc, Rot, 1.0f, 1.0f, 0.0f, SoundAttenuation);
		}
	}
}
