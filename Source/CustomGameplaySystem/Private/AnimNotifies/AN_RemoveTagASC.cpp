// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_RemoveTagASC.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

UAN_RemoveTagASC::UAN_RemoveTagASC()
{
#if WITH_EDITOR
	bShouldFireInEditor = false;
#endif
}

void UAN_RemoveTagASC::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner()))
	{
		ASC->RemoveLooseGameplayTags(TagsToRemove);
	}
}
