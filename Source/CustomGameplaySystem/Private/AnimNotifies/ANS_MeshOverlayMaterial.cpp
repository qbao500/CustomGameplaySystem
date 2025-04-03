// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_MeshOverlayMaterial.h"

void UANS_MeshOverlayMaterial::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;

	MeshComp->SetOverlayMaterial(OverlayMaterial);
}

void UANS_MeshOverlayMaterial::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	if (MeshComp->GetOverlayMaterial() == OverlayMaterial)
	{
		MeshComp->SetOverlayMaterial(nullptr);
	}
}
