// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/CustomRenderingLibrary.h"

#include "Engine/Canvas.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomRenderingLibrary)

void UCustomRenderingLibrary::CanvasDrawText(UCanvas* Canvas, const FSlateFontInfo& FontInfo, const FString& RenderText,
	const FVector2D ScreenPosition, const FVector2D Scale, const FLinearColor& RenderColor, const float Kerning,
	const FLinearColor& ShadowColor, const FVector2D ShadowOffset, const bool bCenterX, const bool bCenterY,
	const bool bOutlined, const FLinearColor& OutlineColor)
{
	if (!Canvas || !Canvas->Canvas || RenderText.IsEmpty()) return;

	FCanvasTextItem TextItem = FCanvasTextItem(ScreenPosition, FText::FromString(RenderText), FontInfo, RenderColor);
	TextItem.HorizSpacingAdjust = Kerning;
	TextItem.ShadowColor = ShadowColor;
	TextItem.ShadowOffset = ShadowOffset;
	TextItem.bCentreX = bCenterX;
	TextItem.bCentreY = bCenterY;
	TextItem.Scale = Scale;
	TextItem.bOutlined = bOutlined;
	TextItem.OutlineColor = OutlineColor;

	Canvas->DrawItem(TextItem);
}
