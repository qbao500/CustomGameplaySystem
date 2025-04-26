// Fill out your copyright notice in the Description page of Project Settings.
// https://www.youtube.com/watch?v=NAOmDWdP9-Y

#pragma once

#include "Feedback/NumberPopUp/CustomNumberPopComponent_Base.h"
#include "NumberPopComponent_RenderTarget.generated.h"

class UNiagaraSystem;

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = ("Number Pop Up"))
class CUSTOMGAMEPLAYSYSTEM_API UNumberPopComponent_RenderTarget : public UCustomNumberPopComponent_Base
{
	GENERATED_BODY()

public:

	virtual void AddNumberPop(const FNumberPopRequest& NewRequest) override;

protected:

	void PrepareData(const FNumberPopRequest& PopRequest, FString& OutString,
		FLinearColor& OutColor, float& OutScaleSize, FVector2D& OutDimension, FVector2D& OutOffset);
	void DrawPopUp(const FString& InString, const FLinearColor& InColor, const float TextScaleSize,
	    const FVector2D InDimension, const FVector2D InOffset, const FVector& WorldLocation);

	UFUNCTION(BlueprintNativeEvent)
	void PrepareDisplayText(const FNumberPopRequest& PopRequest,
		FString& OutString, FLinearColor& OutColor, float& OutScaleSize);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|RenderTarget")
	float RenderTargetDimensionSize = 64.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|RenderTarget")
	TObjectPtr<UMaterialInterface> RenderTargetMaterial;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|RenderTarget")
	FName RenderTargetParameterName = "RT_DamagePop";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Text")
	FLinearColor TextColor = FLinearColor::White;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Text")
	float TextSize = 32.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Text")
	TObjectPtr<UFont> TextFont;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Niagara")
	TObjectPtr<UNiagaraSystem> NiagaraToSpawn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Niagara")
	FVector NiagaraSpawnOffset = FVector(0.0f, 0.0f, 50.0f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Niagara")
	FName NiagaraTextColorName = "TextColor";
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Niagara")
	FName NiagaraSizeDimensionName = "SizeDimension";
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|Niagara")
	FName NiagaraMaterialInterfaceName = "MaterialInterface";
};
