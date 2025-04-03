// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HitReactionComponent.generated.h"

class UFeedbackEffectDataAsset;
class UEmphasisSubsystem;
class UNiagaraSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FDamageReacted, const FHitResult&, HitInfo, const FVector&, FromDirection,
	const AActor*, ReactionCauser, const FDamageEvent&, DamageEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FBlockReacted, const FVector&, EffectLocation, const FVector&, FromDirection,
	const AActor*, ReactionCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FBlockBroken, const FVector&, EffectLocation, const FVector&, FromDirection,
	const AActor*, ReactionCauser, const FDamageEvent&, DamageEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FParryReacted, const FVector&, EffectLocation, const FVector&, FromDirection,
	const AActor*, ReactionCauser);

USTRUCT(BlueprintType)
struct FHitImpact
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableHitStop = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bEnableHitStop"))
	float HitStopDuration = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bEnableHitStop"))
	float HitStopTriggerDelay = 0.05f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UCameraShakeBase> CamShakeClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CamShakeScale = 1.0f;

	bool IsOverriden() const;
	void SetOverriden(const bool bValue);

private:
	
	bool bOverriden = false;
};

USTRUCT()
struct FHitReactionInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "VFX/SFX")
	TObjectPtr<UFeedbackEffectDataAsset> EffectData;
	
	UPROPERTY(EditAnywhere, Category = "SFX")
	FVector2D RandomVolumeMultiplier = FVector2D(0.95f, 1.05f);
	UPROPERTY(EditAnywhere, Category = "SFX")
	FVector2D RandomPitchMultiplier = FVector2D(0.8f, 1.2f);
	UPROPERTY(EditAnywhere, Category = "SFX")
	float StartTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> MontageToPlay;
	UPROPERTY(EditAnywhere, Category = "Montage")
	float MontagePlayRate = 1.0f;

	UPROPERTY(EditAnywhere)
	FHitImpact HitImpactInfo;

	/*UPROPERTY(EditAnywhere, Category = "Camera Shake")
	FShakeInfo ShakeInfo;

	UPROPERTY(EditAnywhere, Category = "Hit Stop")
	FHitStopInfo HitStopInfo;*/
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CUSTOMGAMEPLAYSYSTEM_API UHitReactionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UHitReactionComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, DisplayName = "Trigger Damage Reaction")
	void K2_TriggerDamageReaction(const FHitResult& HitResult, AController* EventInstigator,
		AActor* DamageCauser, const FDamageEvent& DamageEvent);

	void TriggerDamageReaction(const FPointDamageEvent& PointDamageEvent, AController* EventInstigator,
		AActor* DamageCauser, const FDamageEvent& DamageEvent);
	void TriggerBlockReaction(const FVector& Location, AController* EventInstigator, AActor* DamageCauser);
	void TriggerBlockBrokenReaction(const FVector& Location, AController* EventInstigator,
		AActor* DamageCauser, const FDamageEvent& DamageEvent);
	void TriggerParryReaction(const FVector& Location, AController* EventInstigator, AActor* DamageCauser);

	// This must be called before dealing damage
	UFUNCTION(BlueprintCallable)
	void OverrideHitImpact(const FHitImpact& DamageImpact, const FHitImpact& BlockImpact,
		const FHitImpact& BlockBrokenImpact, const FHitImpact& ParryImpact);

	UPROPERTY(BlueprintAssignable)
	FDamageReacted OnDamageReacted;
	UPROPERTY(BlueprintAssignable)
	FBlockReacted OnBlockReacted;
	UPROPERTY(BlueprintAssignable)
	FBlockBroken OnBlockBroken;
	UPROPERTY(BlueprintAssignable)
	FParryReacted OnParryReacted;

	UFUNCTION(BlueprintCallable)
	void RegisterMeshForMontage(USkeletalMeshComponent* NewMesh);

private:

	// Reaction when taking damage. Most of the fields are optional
	UPROPERTY(EditAnywhere, Category = "Damage Reaction")
	FHitReactionInfo DamageReactionInfo;

	// Reaction when blocked successfully. Most of the fields are optional
	UPROPERTY(EditAnywhere, Category = "Block Reaction")
	FHitReactionInfo BlockReactionInfo;

	// Reaction when block broken. Most of the fields are optional
	UPROPERTY(EditAnywhere, Category = "Block Reaction")
	FHitReactionInfo BlockBrokenReactionInfo;

	// Reaction when performed a parry. Most of the fields are optional
	UPROPERTY(EditAnywhere, Category = "Parry Reaction")
	FHitReactionInfo ParryReactionInfo;

	UPROPERTY()
	TObjectPtr<UEmphasisSubsystem> EmphasisSubsystem;
	UPROPERTY()
	TObjectPtr<APlayerCameraManager> CameraManager;

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> MeshForMontage;

	void HandleReaction(const FHitReactionInfo& Info, const FVector& Location, AActor* Causer);

	FHitImpact OverridenDamageImpact = FHitImpact();
	FHitImpact OverridenBlockImpact = FHitImpact();
	FHitImpact OverridenBlockBrokenImpact = FHitImpact();
	FHitImpact OverridenParryImpact = FHitImpact();
};
