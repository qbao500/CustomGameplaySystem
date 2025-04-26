// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "FeedbackEffectDataAsset.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FFeedbackEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<UNiagaraSystem>> Vfx;

	// If true, play 1 random VFX from the list. If false, play all of them at the same time.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bRandomizeVfx = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<USoundBase>> Sfx;

	// If true, play 1 random SFX from the list. If false, play all of them at the same time.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bRandomizeSfx = true;
};

UCLASS(BlueprintType)
class CUSTOMGAMEPLAYSYSTEM_API UFeedbackEffectDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure)
	const FFeedbackEffect& GetEffectFromPhysMat(const UPhysicalMaterial* PhysicalMaterial) const;
	UFUNCTION(BlueprintPure)
	const FFeedbackEffect& GetEffectFromSurface(TEnumAsByte<EPhysicalSurface> PhysicalSurface) const;
	UFUNCTION(BlueprintPure)
	const FFeedbackEffect& GetDefaultEffect() const;

	UFUNCTION(BlueprintPure)
	USoundBase* GetDefaultRandomSound() const;
	UFUNCTION(BlueprintPure)
	UNiagaraSystem* GetDefaultRandomNiagara() const;

	UFUNCTION(BlueprintPure)
	USoundAttenuation* GetSoundAttenuation() const;

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void PlayVFX(const UObject* WorldContextObject, const FHitResult& HitInfo);
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void PlaySFX(const UObject* WorldContextObject, const FHitResult& HitInfo);
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	virtual void PlayEffects(UObject* WorldContextObject, const FHitResult& HitInfo);
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	virtual void PlayDefaultEffects(UObject* WorldContextObject, FVector Location, FRotator Rotation);

protected:

	// If can't retrieve a surface type from physics material, then use the default value
	UPROPERTY(EditAnywhere)
	FFeedbackEffect DefaultEffect;

	UPROPERTY(EditAnywhere, meta = (ForceInlineRow, ShowOnlyInnerProperties))
	TMap<TEnumAsByte<EPhysicalSurface>, FFeedbackEffect> SurfaceEffects;

	UPROPERTY(EditAnywhere, Category = "More SFX")
	TObjectPtr<USoundAttenuation> SoundAttenuation;

private:

	void PlayVFX_Internal(const UObject* WorldContext, const FFeedbackEffect& Effect, const FVector& Loc, const FRotator& Rot);
	void PlaySFX_Internal(const UObject* WorldContext, const FFeedbackEffect& Effect, const FVector& Loc, const FRotator& Rot);
};
