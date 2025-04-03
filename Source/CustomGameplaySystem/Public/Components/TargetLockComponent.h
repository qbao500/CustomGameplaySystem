// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "TargetLockComponent.generated.h"


class UEnemyManagerSubsystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CUSTOMGAMEPLAYSYSTEM_API UTargetLockComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UTargetLockComponent();
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure)
	AActor* GetBestTarget() const;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxDistance = 950.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxFocusAngle = 60.0f;

	// The bigger this number is, the less likely to lock a target at a bigger focus angle
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AngleScoreMultiplier = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Finisher")
	bool bCheckFinisherTarget = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Finisher", meta = (EditCondition = "bCheckFinisherTarget"))
	FGameplayTag FinisherTag = FGameplayTag();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Finisher", meta = (EditCondition = "bCheckFinisherTarget"))
	float FinisherScoreMultiplier = 1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Finisher", meta = (EditCondition = "bCheckFinisherTarget"))
	float FinisherMaxDistanceMultiplier = 2.0f;

	// If this is true, the owner actor needs to implement CombatInterface, and override IsOnAir
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "On Air")
	bool bCheckOnAir = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "On Air", meta = (EditCondition = "bCheckOnAir"))
	float OnAirMaxDistanceMultiplier = 2.0f;

private:

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDebug = false;

	UPROPERTY()
	TObjectPtr<APawn> OwnerPawn;
	UPROPERTY()
	TObjectPtr<APlayerCameraManager> CameraManager;
	UPROPERTY()
	TObjectPtr<UEnemyManagerSubsystem> EnemyManager;
	UPROPERTY()
	TObjectPtr<AActor> BestTarget;

	AActor* FindBestTarget() const;
	bool IsSightToTargetBlocked(const AActor* Target) const;

	float GetOnAirMultiplier() const;

	// Debug helper
	void DebugString(AActor* Actor, const FString& Text, const float HeightOffset = 0.0f, const float FontScale = 1.25f, const FColor Color = FColor::White) const;
};
