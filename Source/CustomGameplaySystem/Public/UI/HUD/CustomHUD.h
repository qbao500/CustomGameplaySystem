// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "CustomHUD.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API ACustomHUD : public AHUD
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<UUserWidget> OverlayWidget;

private:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> OverlayWidgetClass;
};
