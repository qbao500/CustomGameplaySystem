// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AnimationFunctionLibrary.generated.h"

#define ANIM_FL UAnimationFunctionLibrary

UCLASS()
class CUSTOMGAMEPLAYLIBRARY_API UAnimationFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure)
	static FName GetMontageSlotName(UAnimMontage* Montage, int32 TrackIndex);
	
	UFUNCTION(BlueprintPure)
	static float GetMontageRemainTime(const USkeletalMeshComponent* Mesh, const UAnimMontage* Montage);
	
	UFUNCTION(BlueprintPure)
	static int32 GetNotifyAmount(const UAnimSequenceBase* AnimSequence, const FName NotifyName);
	
	UFUNCTION(BlueprintPure)
	static FName GetCurrentStateName(const UAnimInstance* AnimInstance, const FName StateMachineName);
	
	UFUNCTION(BlueprintPure, meta = (CompactNodeTitle = "GetAnimMetaData"))
	static void GetAnimMetaData(const UAnimationAsset* Animation, TArray<UAnimMetaData*>& MetaDatas);
	static const TArray<UAnimMetaData*>& GetAnimMetaData(const UAnimationAsset* Animation);

private:

	static TArray<UAnimMetaData*> EmptyMetaData;
};
