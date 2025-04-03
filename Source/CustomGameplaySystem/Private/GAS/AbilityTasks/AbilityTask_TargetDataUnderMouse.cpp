// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilityTasks/AbilityTask_TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"
#include "FunctionLibraries/CustomHelperFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AbilityTask_TargetDataUnderMouse)

bool FRandomPointsParam::IsValid() const
{
	return bValid;
}

void FRandomPointsParam::SetValid()
{
	bValid = true;
}

UAbilityTask_TargetDataUnderMouse* UAbilityTask_TargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UAbilityTask_TargetDataUnderMouse* NewTask = NewAbilityTask<UAbilityTask_TargetDataUnderMouse>(OwningAbility);

	return NewTask;
}

UAbilityTask_TargetDataUnderMouse* UAbilityTask_TargetDataUnderMouse::CreateRandomPointsAroundMouseTargetData(
	UGameplayAbility* OwningAbility, const FRandomPointsParam& Params)
{
	UAbilityTask_TargetDataUnderMouse* NewTask = NewAbilityTask<UAbilityTask_TargetDataUnderMouse>(OwningAbility);

	NewTask->RandomParam = Params;
	NewTask->RandomParam.SetValid();

	return NewTask;
}

void UAbilityTask_TargetDataUnderMouse::SetupTargetData(FGameplayAbilityTargetDataHandle& DataHandle)
{
	if (!GetPlayerController()) return;
	
	FHitResult CursorHit;
	GetPlayerController()->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	
	if (RandomParam.IsValid())
	{
		/* Get random points around Mouse Cursor */

		// Generate points
		TArray<FVector> Points = HFL::MakeRandomPointsAroundRadius2D(CursorHit.ImpactPoint, RandomParam.NumPoints,
			RandomParam.MinRadius, RandomParam.MaxRadius, RandomParam.bIncludeCenterPoint);
		if (Points.Num() > 0 && RandomParam.bTraceToGround)
		{
			HFL::TraceAndModifyPoints(AbilitySystemComponent.Get(), Points, RandomParam.TraceProfile, RandomParam.TotalTraceDistance);
		}

		FHitResult HitResultWrapper;
		for (const FVector& Point : Points)
		{
			HitResultWrapper.Location = Point;

			// Create Target Data
			FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
			Data->HitResult = HitResultWrapper;
	
			// "Add" function here requires Target Data created with "new".
			DataHandle.Add(Data);
		}
	}
	else
	{
		/* Just Mouse Cursor hit result */
		
		// Create Target Data
		FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
		Data->HitResult = CursorHit;
	
		// "Add" function here requires Target Data created with "new".
		DataHandle.Add(Data);
	}
}