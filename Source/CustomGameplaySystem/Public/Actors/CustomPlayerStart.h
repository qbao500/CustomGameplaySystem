// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerStart.h"
#include "CustomPlayerStart.generated.h"

class UTextRenderComponent;
/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API ACustomPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:

	ACustomPlayerStart(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform) override;

protected:

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTextRenderComponent> TagTextRender;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bHideTagTextIfNone = true;
#endif

#if WITH_EDITOR
	virtual void UpdateTagText() const;
#endif
};
