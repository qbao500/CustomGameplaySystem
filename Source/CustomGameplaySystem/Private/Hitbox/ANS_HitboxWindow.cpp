// Fill out your copyright notice in the Description page of Project Settings.


#include "Hitbox/ANS_HitboxWindow.h"

#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "Hitbox/HitboxComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ANS_HitboxWindow)

void UANS_HitboxWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!HasAuthority(MeshComp)) return;

	if (!HitboxPart.IsValid()) return;

	// Activate hitbox
	if (UHitboxComponent* Comp = MeshComp->GetOwner()->FindComponentByClass<UHitboxComponent>())
	{
		HitboxComp = Comp;
		HitboxComp->ActivateCollision(HitboxPart);
	}

	// Preview
#if WITH_EDITOR
	if (IsPreview(MeshComp) && DebuggingActor)
	{
		PreviewHitboxComp = AActor::GetActorClassDefaultComponent<UHitboxComponent>(DebuggingActor);
		if (PreviewHitboxComp)
		{
			PreviewInfo = PreviewHitboxComp->GetHitboxInfo(HitboxPart);

			// Find mesh of Preview Asset, if applicable
			UPrimitiveComponent* ExternalMesh = nullptr;
			if (!PreviewAssetName.IsEmpty())
			{
				TArray<USceneComponent*> ChildComps;
				MeshComp->GetChildrenComponents(true, ChildComps);
				for (USceneComponent* Child : ChildComps)
				{
					if (!Child) continue;

					const FString PreviewAsset = GetNameSafe(Child->AdditionalStatObject());
					if (!PreviewAsset.Contains(PreviewAssetName)) continue;
					
					// Found Preview Asset, cache its component
					ExternalMesh = Cast<UPrimitiveComponent>(Child);
					break;
				}
			}

			// If found Preview Asset, then use its mesh. Otherwise use main mesh.
			PreviewInfo.CollidingComponent = ExternalMesh ? ExternalMesh : MeshComp;

			PreviewInfo.ValidateInfo();
			PreviewInfo.RefreshSocketTraceLocation();
		}
	}
#endif
}

void UANS_HitboxWindow::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!HasAuthority(MeshComp)) return;

	// Preview
#if WITH_EDITOR
	if (IsPreview(MeshComp) && PreviewHitboxComp)
	{
		PreviewInfo.PreviewDebug(MeshComp, PreviewHitboxComp->GetTraceRadius(), DebugDrawTime);
	}
#endif
}

void UANS_HitboxWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!HasAuthority(MeshComp)) return;

	// Deactivate hitbox and reset pointer
	if (IsCompValid())
	{
		HitboxComp->DeactivateCollision();
		HitboxComp.Reset();
	}

	// Preview
#if WITH_EDITOR
	if (IsPreview(MeshComp))
	{
		PreviewHitboxComp = nullptr;
		PreviewInfo.Reset();
	}
#endif
}

bool UANS_HitboxWindow::IsCompValid() const
{
	return HitboxComp.Get() != nullptr;
}

inline bool UANS_HitboxWindow::HasAuthority(const USkeletalMeshComponent* MeshComp) const
{
	check(MeshComp);
	check(MeshComp->GetOwner());

	return MeshComp->GetOwner()->HasAuthority();
}

bool UANS_HitboxWindow::IsPreview(const UObject* Object)
{
	if (!Object) return false;

	return Object->GetWorld()->IsPreviewWorld();
}
