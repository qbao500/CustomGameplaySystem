// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_RotateActorToTarget.h"

#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "Interfaces/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ANS_RotateActorToTarget)

void UANS_RotateActorToTarget::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	check(MeshComp);

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	// If ignore all rotation, then do nothing
	if (RotateSettings.bIgnoreTargetPitch && RotateSettings.bIgnoreTargetRoll && RotateSettings.bIgnoreTargetYaw)
	{
		return;
	}

	RotateSettings.Duration = TotalDuration;

	switch (RotateMode)
	{
	case ERotateActorMode::CombatTarget:
		if (UKismetSystemLibrary::DoesImplementInterface(Owner, UCombatInterface::StaticClass()))
		{
			if (AActor* Target = ICombatInterface::Execute_GetCombatTarget(Owner))
			{
				Action = UAsyncRotateActorToTarget::AsyncRotateActorToActor(Owner, Target, RotateSettings);
			}
		}
		break;
	case ERotateActorMode::CombatTargetTracking:
		if (UKismetSystemLibrary::DoesImplementInterface(Owner, UCombatInterface::StaticClass()))
		{
			if (AActor* Target = ICombatInterface::Execute_GetCombatTarget(Owner))
			{
				TrackingActor = Target;
				if (MeshComp->bEnableUpdateRateOptimizations)
				{
					bTrackingNeedToBypassRateOptimization = true;
					MeshComp->bEnableUpdateRateOptimizations = false;
				}
			}
		}
		break;
	case ERotateActorMode::SpecifiedRotation:
		Action = UAsyncRotateActorToTarget::AsyncRotateActorToRotation(Owner, Rotation, RotateSettings);
		break;
	default: ;
	}

	if (Action.Get())
	{
		Action->Activate();
	}
}

void UANS_RotateActorToTarget::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	
	check(MeshComp);

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	switch (RotateMode)
	{
	case ERotateActorMode::CombatTargetTracking:
		if (TrackingActor.Get())
		{
			const FQuat CurrentQuat = Owner->GetActorQuat();
			const FQuat TargetQuat = (TrackingActor->GetActorLocation() - Owner->GetActorLocation()).ToOrientationQuat();
			FRotator NewRotation = FMath::QInterpTo(CurrentQuat, TargetQuat, FrameDeltaTime, RotationSpeed).Rotator();
			RotateSettings.ModifyRotator(NewRotation);
			Owner->SetActorRotation(NewRotation);
		}
		break;
	default:
		break;
	}
}

void UANS_RotateActorToTarget::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (Action.Get())
	{
		if (bStopRotateWhenAnimationEnded)
		{
			Action->Cancel();
		}

		Action.Reset();
	}

	TrackingActor.Reset();
	if (bTrackingNeedToBypassRateOptimization)
	{
		bTrackingNeedToBypassRateOptimization = false;
		MeshComp->bEnableUpdateRateOptimizations = true;
	}
}

FString UANS_RotateActorToTarget::GetNotifyName_Implementation() const
{
	FString Name = "Rotate (";

	switch (RotateMode)
	{
	case ERotateActorMode::SpecifiedRotation:
		Name += Rotation.ToCompactString();
		break;
	default:
		Name += PLFL::EnumToString(RotateMode);
		break;
	}

	Name += ")";
	
	return Name;
}

#if WITH_EDITOR
bool UANS_RotateActorToTarget::CanEditChange(const FProperty* InProperty) const
{
	const bool Parent = Super::CanEditChange(InProperty);
	
	const FName PropertyName = InProperty->GetFName();
	
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, Rotation))
	{
		return RotateMode == ERotateActorMode::SpecifiedRotation;
	}
	
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, RotationSpeed))
	{
		return RotateMode == ERotateActorMode::CombatTargetTracking;
	}
	
	return Parent;
}
#endif