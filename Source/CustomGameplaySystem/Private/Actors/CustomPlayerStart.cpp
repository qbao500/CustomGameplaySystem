// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CustomPlayerStart.h"

#if WITH_EDITOR
#include "Components/TextRenderComponent.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomPlayerStart)

ACustomPlayerStart::ACustomPlayerStart(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	TagTextRender = CreateEditorOnlyDefaultSubobject<UTextRenderComponent>("TagTextRender");
	TagTextRender->SetupAttachment(GetRootComponent());
	TagTextRender->SetHiddenInGame(true);
	TagTextRender->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	TagTextRender->SetWorldSize(100.0f);
	TagTextRender->SetHorizontalAlignment(EHTA_Center);
	TagTextRender->SetVerticalAlignment(EVRTA_TextCenter);
#endif
}

void ACustomPlayerStart::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	UpdateTagText();
#endif
}

#if WITH_EDITOR
void ACustomPlayerStart::UpdateTagText() const
{
	if (TagTextRender)
	{
		const FString TagText = PlayerStartTag.IsNone() ? "No Tag" : PlayerStartTag.ToString();
		TagTextRender->SetText(FText::FromString(TagText));
	}
}
#endif