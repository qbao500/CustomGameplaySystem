// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HitReactionComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "DataAssets/FeedbackEffectDataAsset.h"
#include "Engine/DamageEvents.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Subsystems/EmphasisSubsystem.h"

bool FHitImpact::IsOverriden() const
{
	return bOverriden;
}

void FHitImpact::SetOverriden(const bool bValue)
{
	bOverriden = bValue;
}

UHitReactionComponent::UHitReactionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UHitReactionComponent::BeginPlay()
{
	Super::BeginPlay();

	EmphasisSubsystem = UWorld::GetSubsystem<UEmphasisSubsystem>(GetWorld());
	CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
}

void UHitReactionComponent::K2_TriggerDamageReaction(const FHitResult& HitResult, AController* EventInstigator,
	AActor* DamageCauser, const FDamageEvent& DamageEvent)
{
	FVector FromDirection = FVector(0);
	if (DamageCauser)
	{
		FromDirection = UKismetMathLibrary::GetDirectionUnitVector(DamageCauser->GetActorLocation(), GetOwner()->GetActorLocation());
	}

	// Handle override
	FHitReactionInfo ReactionInfo = DamageReactionInfo;
	if (OverridenDamageImpact.IsOverriden())
	{
		ReactionInfo.HitImpactInfo = OverridenDamageImpact;
	}
	
	HandleReaction(ReactionInfo, HitResult.ImpactPoint, EventInstigator ? EventInstigator->GetPawn() : DamageCauser);

	// Event
	OnDamageReacted.Broadcast(HitResult, FromDirection, EventInstigator->GetPawn(), DamageEvent);

	// Reset
	OverridenDamageImpact = FHitImpact();
}

void UHitReactionComponent::TriggerDamageReaction(const FPointDamageEvent& PointDamageEvent, AController* EventInstigator,
	AActor* DamageCauser, const FDamageEvent& DamageEvent)
{
	const FHitResult& HitInfo = PointDamageEvent.HitInfo;
	const FVector FromDirection = UKismetMathLibrary::GetDirectionUnitVector(PointDamageEvent.ShotDirection, GetOwner()->GetActorLocation());

	// Handle override
	FHitReactionInfo ReactionInfo = DamageReactionInfo;
	if (OverridenDamageImpact.IsOverriden())
	{
		ReactionInfo.HitImpactInfo = OverridenDamageImpact;
	}
	
	HandleReaction(ReactionInfo, HitInfo.ImpactPoint, EventInstigator ? EventInstigator->GetPawn() : DamageCauser);

	// Event
	OnDamageReacted.Broadcast(HitInfo, FromDirection, EventInstigator->GetPawn(), DamageEvent);

	// Reset
	OverridenDamageImpact = FHitImpact();
}

void UHitReactionComponent::TriggerBlockReaction(const FVector& Location, AController* EventInstigator, AActor* DamageCauser)
{
	const FVector FromDirection = UKismetMathLibrary::GetDirectionUnitVector(DamageCauser ? DamageCauser->GetActorLocation() : Location,
		GetOwner()->GetActorLocation());

	// Handle override
	FHitReactionInfo ReactionInfo = BlockReactionInfo;
	if (OverridenBlockImpact.IsOverriden())
	{
		ReactionInfo.HitImpactInfo = OverridenBlockImpact;
	}

	HandleReaction(ReactionInfo, Location, EventInstigator ? EventInstigator->GetPawn() : DamageCauser);

	// Event
	OnBlockReacted.Broadcast(Location, FromDirection, DamageCauser);

	// Reset
	OverridenDamageImpact = FHitImpact();
}

void UHitReactionComponent::TriggerBlockBrokenReaction(const FVector& Location, AController* EventInstigator,
	AActor* DamageCauser, const FDamageEvent& DamageEvent)
{
	const FVector FromDirection = UKismetMathLibrary::GetDirectionUnitVector(DamageCauser ? DamageCauser->GetActorLocation() : Location,
		GetOwner()->GetActorLocation());

	// Handle override
	FHitReactionInfo ReactionInfo = BlockBrokenReactionInfo;
	if (OverridenBlockBrokenImpact.IsOverriden())
	{
		ReactionInfo.HitImpactInfo = OverridenBlockBrokenImpact;
	}

	HandleReaction(ReactionInfo, Location, EventInstigator ? EventInstigator->GetPawn() : DamageCauser);

	// Event
	OnBlockBroken.Broadcast(Location, FromDirection, DamageCauser, DamageEvent);

	// Reset
	OverridenDamageImpact = FHitImpact();
}

void UHitReactionComponent::TriggerParryReaction(const FVector& Location, AController* EventInstigator, AActor* DamageCauser)
{
	const FVector FromDirection = UKismetMathLibrary::GetDirectionUnitVector(DamageCauser ? DamageCauser->GetActorLocation() : Location,
		GetOwner()->GetActorLocation());

	// Handle override
	FHitReactionInfo ReactionInfo = ParryReactionInfo;
	if (OverridenParryImpact.IsOverriden())
	{
		ReactionInfo.HitImpactInfo = OverridenParryImpact;
	}

	HandleReaction(ReactionInfo, Location, EventInstigator ? EventInstigator->GetPawn() : DamageCauser);

	// Event
	OnParryReacted.Broadcast(Location, FromDirection, EventInstigator->GetPawn());

	// Reset
	OverridenDamageImpact = FHitImpact();
}

void UHitReactionComponent::OverrideHitImpact(const FHitImpact& DamageImpact, const FHitImpact& BlockImpact,
	const FHitImpact& BlockBrokenImpact, const FHitImpact& ParryImpact)
{
	OverridenDamageImpact = DamageImpact;
	OverridenBlockImpact = BlockImpact;
	OverridenBlockBrokenImpact = BlockBrokenImpact;
	OverridenParryImpact = ParryImpact;

	OverridenDamageImpact.SetOverriden(true);
	OverridenBlockImpact.SetOverriden(true);
	OverridenBlockBrokenImpact.SetOverriden(true);
	OverridenParryImpact.SetOverriden(true);
}

void UHitReactionComponent::RegisterMeshForMontage(USkeletalMeshComponent* NewMesh)
{
	MeshForMontage = NewMesh;
}

void UHitReactionComponent::HandleReaction(const FHitReactionInfo& Info, const FVector& Location, AActor* Causer)
{
	// VFX / SFX
	if (Info.EffectData)
	{
		Info.EffectData->PlayDefaultEffects(this, Location, UKismetMathLibrary::RandomRotator());
	}

	// Montage
	if (Info.MontageToPlay && MeshForMontage && MeshForMontage->GetAnimInstance())
	{
		MeshForMontage->GetAnimInstance()->Montage_Play(Info.MontageToPlay, Info.MontagePlayRate, EMontagePlayReturnType::Duration);
	}

	// Camera Shake
	if (CameraManager && Info.HitImpactInfo.CamShakeClass)
	{
		CameraManager->StartCameraShake(Info.HitImpactInfo.CamShakeClass, Info.HitImpactInfo.CamShakeScale);
	}

	// Hit Stop
	if (EmphasisSubsystem && Info.HitImpactInfo.bEnableHitStop && Causer)
	{
		EmphasisSubsystem->HitStop(Causer, GetOwner(), Info.HitImpactInfo.HitStopDuration, Info.HitImpactInfo.HitStopTriggerDelay);
	}
}
