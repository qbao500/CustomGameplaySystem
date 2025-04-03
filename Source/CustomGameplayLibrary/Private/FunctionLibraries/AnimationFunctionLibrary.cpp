// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/AnimationFunctionLibrary.h"

#include "Animation/AnimNode_StateMachine.h"
#include "Animation/AnimMetaData.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimationFunctionLibrary)

TArray<UAnimMetaData*> UAnimationFunctionLibrary::EmptyMetaData;

FName UAnimationFunctionLibrary::GetMontageSlotName(UAnimMontage* Montage, const int32 TrackIndex)
{
	if (!Montage || !Montage->SlotAnimTracks.IsValidIndex(TrackIndex)) return FName();

	return Montage->SlotAnimTracks[TrackIndex].SlotName;
}

float UAnimationFunctionLibrary::GetMontageRemainTime(const USkeletalMeshComponent* Mesh, const UAnimMontage* Montage)
{
	if (!Mesh || !Montage) return 0;

	const UAnimInstance* Instance = Mesh->GetAnimInstance();
	if (!Instance) return 0;

	const float Total = Montage->GetPlayLength() / Montage->RateScale;
	const float Elapsed = Instance->Montage_GetPosition(Montage) / Montage->RateScale;

	return Total - Elapsed;
}

int32 UAnimationFunctionLibrary::GetNotifyAmount(const UAnimSequenceBase* AnimSequence, const FName NotifyName)
{
	if (!AnimSequence || NotifyName.IsNone()) return 0;

	int32 Count = 0;
	for (const FAnimNotifyEvent& Notify : AnimSequence->Notifies)
	{
		if (Notify.NotifyName.IsEqual(NotifyName))
		{
			++Count;
		}
	}

	return Count;
}

FName UAnimationFunctionLibrary::GetCurrentStateName(const UAnimInstance* AnimInstance, const FName StateMachineName)
{
	if (!AnimInstance) return FName();

	const FAnimNode_StateMachine* StateMachine = AnimInstance->GetStateMachineInstanceFromName(StateMachineName);
	if (!StateMachine) return FName();

	return StateMachine->GetCurrentStateName();
}

void UAnimationFunctionLibrary::GetAnimMetaData(const UAnimationAsset* Animation, TArray<UAnimMetaData*>& MetaDatas)
{
	if (Animation)
	{
		MetaDatas = Animation->GetMetaData();
	}
}

const TArray<UAnimMetaData*>& UAnimationFunctionLibrary::GetAnimMetaData(const UAnimationAsset* Animation)
{
	return Animation ? Animation->GetMetaData() : EmptyMetaData;
}

