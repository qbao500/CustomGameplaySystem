// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "WeaponActor.generated.h"

class UFeedbackEffectDataAsset;

UCLASS(Abstract)
class CUSTOMGAMEPLAYSYSTEM_API AWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	

	AWeaponActor();

	virtual void BeginPlay() override;

	void UnequipWeapon();

	UFUNCTION(BlueprintPure)
	const FGameplayTag& GetWeaponTag() const;
	UFUNCTION(BlueprintCallable)
	void SetWeaponTag(const FGameplayTag NewTag);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	float PlayWeaponMontage(UAnimMontage* Montage, const float PlayRate = 1.0f);

	UFUNCTION(BlueprintCallable)
	void TriggerMeleeHitEffect(const FHitResult& HitInfo);

	UFUNCTION(BlueprintCallable)
	void EquipInstantly();
	UFUNCTION(BlueprintCallable)
	void UnequipInstantly();

protected:

	UFUNCTION(BlueprintImplementableEvent)
	void OnEquipped();
	UFUNCTION(BlueprintImplementableEvent)
	void OnUnequipped();

private:

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFeedbackEffectDataAsset> EquipEffectData;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFeedbackEffectDataAsset> MeleeHitEffectData;
	
	FGameplayTag WeaponTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly)
	float SwapDuration = 0.2f;
	FTimerHandle EquipDelayTimer;
	void EquipDelayed();
};
