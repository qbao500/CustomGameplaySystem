// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilityTasks/AbilityTask_Tick.h"

UAbilityTask_Tick::UAbilityTask_Tick(const FObjectInitializer& ObjectInitializer)
{
	bTickingTask = true;
}

UAbilityTask_Tick* UAbilityTask_Tick::AbilityTaskOnTick(UGameplayAbility* OwningAbility)
{
	UAbilityTask_Tick* MyObj = NewAbilityTask<UAbilityTask_Tick>(OwningAbility);
	return MyObj;
}

void UAbilityTask_Tick::TickTask(const float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnTick.Broadcast(DeltaTime);
	}
}
