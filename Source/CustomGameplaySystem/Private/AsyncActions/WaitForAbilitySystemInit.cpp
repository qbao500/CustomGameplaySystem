// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncActions/WaitForAbilitySystemInit.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WaitForAbilitySystemInit)

UWaitForAbilitySystemInit* UWaitForAbilitySystemInit::WaitForAbilitySystemInit(APawn* PawnWithCorePawnComponent)
{
	if (!PawnWithCorePawnComponent) return nullptr;

	UCustomCorePawnComponent* CorePawnComp = UCustomCorePawnComponent::FindCorePawnComponent(PawnWithCorePawnComponent);
	if (!CorePawnComp) return nullptr;

	UWaitForAbilitySystemInit* Node = NewObject<UWaitForAbilitySystemInit>();
	Node->CorePawnComponent = CorePawnComp;
	Node->RegisterWithGameInstance(PawnWithCorePawnComponent);
	
	return Node;
}

void UWaitForAbilitySystemInit::Activate()
{
	Super::Activate();

	FAbilityComponentInitialized::FDelegate Del;
	Del.BindDynamic(this, &UWaitForAbilitySystemInit::AbilitySystemInitialized);
	CorePawnComponent->OnAbilitySystemInitialized_RegisterAndCall(Del);
}

void UWaitForAbilitySystemInit::AbilitySystemInitialized(UCustomAbilitySystemComponent* CustomASC)
{
	OnAbilitySystemInitialized.Broadcast(CustomASC);

	Cancel();
}
