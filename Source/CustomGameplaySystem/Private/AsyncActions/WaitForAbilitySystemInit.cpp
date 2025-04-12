// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncActions/WaitForAbilitySystemInit.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WaitForAbilitySystemInit)

UWaitForAbilitySystemInit* UWaitForAbilitySystemInit::WaitForAbilityComponentInit(APawn* PawnWithCorePawnComponent)
{
	if (!PawnWithCorePawnComponent) return nullptr;

	UCustomCorePawnComponent* CorePawn = UCustomCorePawnComponent::FindCorePawnComponent(PawnWithCorePawnComponent);
	if (!CorePawn) return nullptr;

	UWaitForAbilitySystemInit* Node = NewObject<UWaitForAbilitySystemInit>();
	Node->CorePawnComp = CorePawn;
	Node->RegisterWithGameInstance(PawnWithCorePawnComponent);
	
	return Node;
}

UWaitForAbilitySystemInit* UWaitForAbilitySystemInit::WaitForAbilityComponentInitWithCoreComp(
	UCustomCorePawnComponent* CorePawnComponent)
{
	if (!CorePawnComponent) return nullptr;

	UWaitForAbilitySystemInit* Node = NewObject<UWaitForAbilitySystemInit>();
	Node->CorePawnComp = CorePawnComponent;
	Node->RegisterWithGameInstance(CorePawnComponent);
	
	return Node;
}

void UWaitForAbilitySystemInit::Activate()
{
	Super::Activate();

	FAbilityComponentInitialized::FDelegate Del;
	Del.BindDynamic(this, &UWaitForAbilitySystemInit::AbilitySystemInitialized);
	CorePawnComp->OnAbilitySystemInitialized_RegisterAndCall(Del);
}

void UWaitForAbilitySystemInit::AbilitySystemInitialized(UCustomAbilitySystemComponent* CustomASC)
{
	OnAbilitySystemInitialized.Broadcast(CustomASC);

	Cancel();
}
