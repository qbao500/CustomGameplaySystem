// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CustomRenderingLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYLIBRARY_API UCustomRenderingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Draws text on the Canvas. This function offers changing Text Size (in FontInfo).
	 *
	 * @param Canvas					Canvas to render the text on.
	 * @param FontInfo					Font to use when rendering the text. Only Font and Size are used.
	 * @param RenderText				Text to render on the Canvas.
	 * @param ScreenPosition			Screen space position to render the text.
	 * @param Scale						Scale text size.
	 * @param RenderColor				Color to render the text.
	 * @param Kerning					Horizontal spacing adjustment to modify the spacing between each letter.
	 * @param ShadowColor				Color to render the shadow of the text.
	 * @param ShadowOffset				Pixel offset relative to the screen space position to render the shadow of the text.
	 * @param bCenterX					If true, then interpret the screen space position X coordinate as the center of the rendered text.
	 * @param bCenterY					If true, then interpret the screen space position Y coordinate as the center of the rendered text.
	 * @param bOutlined					If true, then the text should be rendered with an outline.
	 * @param OutlineColor				Color to render the outline for the text.
	 */	
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "RenderText, RenderColor, ShadowColor, OutlineColor"))
	static void CanvasDrawText(UCanvas* Canvas, const FSlateFontInfo& FontInfo, const FString& RenderText, const FVector2D ScreenPosition,
		const FVector2D Scale = FVector2D(1, 1), const FLinearColor& RenderColor = FLinearColor::White, const float Kerning = 0.0f,
		const FLinearColor& ShadowColor = FLinearColor::Black, const FVector2D ShadowOffset = FVector2D(1, 1),
		const bool bCenterX = true, const bool bCenterY = true, const bool bOutlined = false, const FLinearColor& OutlineColor = FLinearColor::Black);
};
