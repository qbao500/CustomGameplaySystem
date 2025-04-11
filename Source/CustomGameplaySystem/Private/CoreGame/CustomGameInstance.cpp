// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreGame/CustomGameInstance.h"

#include "CustomGameplayTags.h"
#include "GameplayTagContainer.h"
#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomGameInstance)

void UCustomGameInstance::Init()
{
	Super::Init();

	// Register our custom init states
	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);
	if (ensure(ComponentManager))
	{
		ComponentManager->RegisterInitState(CustomTags::InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(CustomTags::InitState_DataAvailable, false, CustomTags::InitState_Spawned);
		ComponentManager->RegisterInitState(CustomTags::InitState_DataInitialized, false, CustomTags::InitState_DataAvailable);
		ComponentManager->RegisterInitState(CustomTags::InitState_GameplayReady, false, CustomTags::InitState_DataInitialized);
	}
}
