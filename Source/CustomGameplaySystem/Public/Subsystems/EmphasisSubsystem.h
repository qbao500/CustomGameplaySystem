// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EmphasisSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UEmphasisSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	// Slow motion
	UFUNCTION(BlueprintCallable)
	void RunConstantSlomo(float Rate, float Duration);
	UFUNCTION(BlueprintCallable)
	void RunDecayingSlomo(float Rate, float Duration, float FirstDelay = 0);
	UFUNCTION(BlueprintCallable)
	void CancelSlomo();
	UFUNCTION(BlueprintCallable)
	bool IsInSlomo() const;

	// Set -1 to reset overriden value. Default value is 0.75
	UFUNCTION(BlueprintCallable)
	void OverrideSlomoGlobalPitch(const float Pitch);
	UFUNCTION(BlueprintPure)
	// If overriden, return the corresponding value. Otherwise, return 0.75
	float GetOverridenGlobalPitch() const;

	// Hit stop
	UFUNCTION(BlueprintCallable)
	void HitStop(AActor* Caller, AActor* Target, const float Duration, const float FirstDelay = 0.0f);
	UFUNCTION(BlueprintCallable)
	void CancelAllHitStop();

	// Camera shake
	// Please use this function always, since it will take settings (for real players) for camera shake scale
	UFUNCTION(BlueprintCallable)
	UCameraShakeBase* StartCameraShake(TSubclassOf<UCameraShakeBase> ShakeClass, float Scale = 1.0f,
		ECameraShakePlaySpace PlaySpace = ECameraShakePlaySpace::CameraLocal, FRotator UserPlaySpaceRot = FRotator::ZeroRotator);

	UFUNCTION(BlueprintPure)
	static UEmphasisSubsystem* GetEmphasisSubsystem(const AActor* ContextActor);

private:

	UPROPERTY()
	TObjectPtr<APlayerCameraManager> CameraManager;

	void StopSlomo() const;
	void StartSlomoDecay(float InDuration, float InRate);
	void ReturningToNormalTime(float Duration);
	void RealSlomoMechanic(const float InRate) const;
	
	FTimerHandle SlomoHandle;
	float SlomoStartTime = 0.0f;
	float SlomoTickTime = 0.0f;
	float CurrentSlomoRate = 0.0f;
	float DurationTracker = 0;
	
	const float SlomoTickRate = 0.1f;

	float OverridenGlobalPitch = INDEX_NONE;

	void BeginHitStop(AActor* Caller, AActor* Target, const float Duration);
	void FinishHitStop(AActor* Caller, AActor* Target);
	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitStoppingActors;

	// Helper
	FTimerManager& GetTimerManager() const;
};
