// Fill out your copyright notice in the Description page of Project Settings.


#include "Feedback/NumberPopUp/NumberPopComponent_RenderTarget.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/Canvas.h"
#include "FunctionLibraries/CustomHelperFunctionLibrary.h"
#include "FunctionLibraries/CustomRenderingLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NumberPopComponent_RenderTarget)

void UNumberPopComponent_RenderTarget::AddNumberPop(const FNumberPopRequest& NewRequest)
{
	FString DisplayString;
	FLinearColor DisplayColor;
	FVector2D Dimension, Offset;
	float ScaleSize = 1.0f;
	PrepareData(NewRequest, DisplayString, DisplayColor, ScaleSize, Dimension, Offset);

	DrawPopUp(DisplayString, DisplayColor, ScaleSize, Dimension, Offset, NewRequest.WorldLocation);
}

void UNumberPopComponent_RenderTarget::PrepareData(const FNumberPopRequest& PopRequest, FString& OutString,
	FLinearColor& OutColor, float& OutScaleSize, FVector2D& OutDimension, FVector2D& OutOffset)
{
	PrepareDisplayText(PopRequest, OutString, OutColor, OutScaleSize);

	// Width is defined by text length. Height is constant.
	OutDimension.X = OutString.Len() * RenderTargetDimensionSize;
	OutDimension.Y = RenderTargetDimensionSize;

	// Basically mean the Render Target is in the center of material
	OutOffset = OutDimension / 2.0f;
}

void UNumberPopComponent_RenderTarget::DrawPopUp(const FString& InString, const FLinearColor& InColor,
	const float TextScaleSize, const FVector2D InDimension, const FVector2D InOffset, const FVector& WorldLocation)
{
	if (!RenderTargetMaterial || !TextFont || !NiagaraToSpawn) return;

	// Init
	UTextureRenderTarget2D* RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(
		this, InDimension.X, InDimension.Y, RTF_R8);
	check(RenderTarget);
	UMaterialInstanceDynamic* MID = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, RenderTargetMaterial);
	check(MID);
	MID->SetTextureParameterValue(RenderTargetParameterName, RenderTarget);

	// Draw render target
	UCanvas* DrawCanvas;
	FVector2D DrawSize;
	FDrawToRenderTargetContext DrawContext;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, RenderTarget, DrawCanvas, DrawSize, DrawContext);
	check(DrawCanvas);
	FSlateFontInfo FontInfo = FSlateFontInfo(TextFont, TextSize);
	UCustomRenderingLibrary::CanvasDrawText(DrawCanvas, FontInfo, InString, InOffset, FVector2D(TextScaleSize), InColor,
		0, FLinearColor::Black, FVector2D(0), true, true, false);
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, DrawContext);

	// Spawn Niagara
	UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, NiagaraToSpawn,
		WorldLocation + NiagaraSpawnOffset, FRotator(0), FVector(1), true, false);
	check(NiagaraComp);
	
	NiagaraComp->SetVariableLinearColor(NiagaraTextColorName, InColor);
	NiagaraComp->SetVariableVec2(NiagaraSizeDimensionName, InDimension);
	NiagaraComp->SetVariableMaterial(NiagaraMaterialInterfaceName, MID);

	NiagaraComp->Activate();
}

void UNumberPopComponent_RenderTarget::PrepareDisplayText_Implementation(const FNumberPopRequest& PopRequest,
	FString& OutString, FLinearColor& OutColor, float& OutScaleSize)
{
	OutString = PopRequest.StringToDisplay;

	OutColor = TextColor;

	OutScaleSize = 1.0f;
}
