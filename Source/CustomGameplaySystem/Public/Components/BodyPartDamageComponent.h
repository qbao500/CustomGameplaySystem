// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "BodyPartDamageComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CUSTOMGAMEPLAYSYSTEM_API UBodyPartDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UBodyPartDamageComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	void CalculateBodyPartDamage(const FHitResult& HitInfo, UPARAM(ref) float& DamageRef) const;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ForceInlineRow))
	TMap<TEnumAsByte<EPhysicalSurface>, float> DamageMultiplier;
};
