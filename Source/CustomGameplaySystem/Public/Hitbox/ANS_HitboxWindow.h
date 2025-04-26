// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CustomGameplayTags.h"
#include "GameplayTagContainer.h"
#include "HitboxComponent.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_HitboxWindow.generated.h"

class UHitboxComponent;

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UANS_HitboxWindow : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:

	// HitboxPart info is defined in HitboxComponent of owner actor
	UPROPERTY(EditAnywhere, meta = (Categories = "Hitbox.Part"))
	FGameplayTag HitboxPart = CustomTags::Hitbox_Part;

#if WITH_EDITORONLY_DATA
	// This Actor needs to have Hitbox Component attached in BP
	UPROPERTY(EditAnywhere, AdvancedDisplay)
	TSubclassOf<AActor> DebuggingActor;
	// If hitbox check is on other mesh (like weapon), then add that mesh as Preview Asset and enter its name here
	// If hitbox check is on viewing mesh, then you can leave this blank
	UPROPERTY(EditAnywhere, AdvancedDisplay)
	FString PreviewAssetName = FString();
	UPROPERTY(EditAnywhere, AdvancedDisplay)
	float DebugDrawTime = 1.5f;
#endif
	
private:

	TWeakObjectPtr<UHitboxComponent> HitboxComp;

	bool IsCompValid() const;
	bool HasAuthority(const USkeletalMeshComponent* MeshComp) const;

	bool IsPreview(const UObject* Object);

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	const UHitboxComponent* PreviewHitboxComp;
	FHitboxInfo PreviewInfo = FHitboxInfo();
#endif
};
