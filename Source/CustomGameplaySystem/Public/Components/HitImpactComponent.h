// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitReactionComponent.h"
#include "Components/ActorComponent.h"
#include "HitImpactComponent.generated.h"

class UEmphasisSubsystem;

/**
 *  
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CUSTOMGAMEPLAYSYSTEM_API UHitImpactComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UHitImpactComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintPure = "false")
	void TriggerImpact(const FHitImpact& ImpactInfo, AActor* HitActor) const;

private:

	UPROPERTY()
	TObjectPtr<UEmphasisSubsystem> EmphasisSubsystem;
	UPROPERTY()
	TObjectPtr<APlayerCameraManager> CameraManager;
};
