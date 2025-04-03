// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HitImpactComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Subsystems/EmphasisSubsystem.h"

UHitImpactComponent::UHitImpactComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UHitImpactComponent::BeginPlay()
{
	Super::BeginPlay();

	EmphasisSubsystem = UWorld::GetSubsystem<UEmphasisSubsystem>(GetWorld());

	if (const APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		CameraManager = OwnerPawn->GetLocalViewingPlayerController()->PlayerCameraManager;
	}
	else
	{
		CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	}
}

void UHitImpactComponent::TriggerImpact(const FHitImpact& ImpactInfo, AActor* HitActor) const
{
	// Camera Shake
	if (CameraManager && ImpactInfo.CamShakeClass)
	{
		CameraManager->StartCameraShake(ImpactInfo.CamShakeClass, ImpactInfo.CamShakeScale);
	}

	// Hit Stop
	if (EmphasisSubsystem && ImpactInfo.bEnableHitStop && HitActor)
	{
		EmphasisSubsystem->HitStop(GetOwner(), HitActor, ImpactInfo.HitStopDuration, ImpactInfo.HitStopTriggerDelay);
	}
}
