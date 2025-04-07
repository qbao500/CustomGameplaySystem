// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "TargetLockComponent.generated.h"


class UEnemyManagerSubsystem;

UENUM(BlueprintType)
enum class ETargetLockMethod : uint8
{
	// Check distance from Pawn, look angle and sight from Camera
	FromCamera,
	// Check distance and sight from Pawn, look angle from Pawn forward vector
	FromOwnerPawn,
	// Check distance and sight from Pawn, but no look angle
	FromOwnerPawnNoLook,
};

/**
 * A component that uses EnemyManagerSubsystem to find the best target enemy.
 * Must put in Pawn or Character that uses for Player.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CUSTOMGAMEPLAYSYSTEM_API UTargetLockComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UTargetLockComponent();

	//~ Begin UActorComponent interface
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override final;
	//~ End UActorComponent interface

	UFUNCTION(BlueprintPure)
	virtual AActor* GetBestTarget() const;

	UFUNCTION(BlueprintPure)
	float GetMaxDistance() const;
	UFUNCTION(BlueprintPure)
	float GetLocationForwardOffset() const;

	UFUNCTION(BlueprintCallable)
	void SetMaxFocusAngle(const float NewValue);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ETargetLockMethod TargetLockMethod = ETargetLockMethod::FromCamera;

	// Off set the location of the owner in forward direction
	// Negative value means backward
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float LocationForwardOffset = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxDistance = 950.0f;

	// The maximum angle to find a target from camera
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxFocusAngle = 60.0f;

	// The bigger this number is, the less likely to lock a target at a bigger focus angle
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AngleScoreMultiplier = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCheckLineOfSight = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bCheckLineOfSight"))
	TEnumAsByte<ECollisionChannel> LineOfSightChannel = ECC_Visibility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Finisher")
	bool bCheckFinisherTarget = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Finisher", meta = (EditCondition = "bCheckFinisherTarget"))
	FGameplayTag FinisherTag = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Finisher", meta = (EditCondition = "bCheckFinisherTarget"))
	float FinisherScoreMultiplier = 1.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Finisher", meta = (EditCondition = "bCheckFinisherTarget"))
	float FinisherMaxDistanceMultiplier = 2.0f;

	// If this is true, the owner actor needs to implement CombatInterface, and override IsOnAir
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "On Air")
	bool bCheckOnAir = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "On Air", meta = (EditCondition = "bCheckOnAir"))
	float OnAirMaxDistanceMultiplier = 2.0f;

	UPROPERTY()
	TObjectPtr<APawn> OwnerPawn;
	UPROPERTY()
	TObjectPtr<APlayerCameraManager> CameraManager;
	UPROPERTY()
	TObjectPtr<UEnemyManagerSubsystem> EnemyManager;
	UPROPERTY()
	TWeakObjectPtr<AActor> BestTarget;

	virtual void TickFindTarget(float DeltaTime);
	AActor* FindBestTarget(const TSet<AActor*>& IgnoredActors = TSet<AActor*>()) const;
	virtual bool ShouldCheckLookAngle() const;
	virtual bool IsSightToTargetBlocked(const AActor* Target) const;

	float GetOnAirMultiplier() const;

	FVector GetOwnerLocation() const;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDebug = false;
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDebugOnyScore = false;
	UPROPERTY(EditAnywhere, Category = "Debug")
	float DebugStringHeightOffset = 20.0f;

	// Debug helper
	void DebugString(AActor* Actor, const FString& Text, const float HeightOffset = 0.0f, const float FontScale = 1.25f, const FColor Color = FColor::White) const;
	float GetDebugTime() const;
};
