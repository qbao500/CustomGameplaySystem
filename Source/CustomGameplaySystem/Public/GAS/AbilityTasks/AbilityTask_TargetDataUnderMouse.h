// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilityTask_CreateTargetData.h"
#include "AbilityTask_TargetDataUnderMouse.generated.h"

USTRUCT(BlueprintType)
struct FRandomPointsParam
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumPoints = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinRadius = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxRadius = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIncludeCenterPoint = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTraceToGround = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCollisionProfileName TraceProfile = FCollisionProfileName();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TotalTraceDistance = 1000.0f;

	bool IsValid() const;
	void SetValid();

private:

	bool bValid = false;
};



/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UAbilityTask_TargetDataUnderMouse : public UAbilityTask_CreateTargetData
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"), BlueprintInternalUseOnly)
	static UAbilityTask_TargetDataUnderMouse* CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"), BlueprintInternalUseOnly)
	static UAbilityTask_TargetDataUnderMouse* CreateRandomPointsAroundMouseTargetData(UGameplayAbility* OwningAbility, const FRandomPointsParam& Params);

protected:

	virtual void SetupTargetData(FGameplayAbilityTargetDataHandle& DataHandle) override;

private:

	FRandomPointsParam RandomParam = FRandomPointsParam();
};
