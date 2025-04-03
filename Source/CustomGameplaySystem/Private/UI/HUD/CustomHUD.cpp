// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/CustomHUD.h"

#include "Blueprint/UserWidget.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"

void ACustomHUD::BeginPlay()
{
	Super::BeginPlay();

	if (!OverlayWidgetClass)
	{
		PLFL::PrintWarning("There's no OverlayWidgetClass assigned in CustomHUD!");
		return;
	}
	
	OverlayWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), OverlayWidgetClass);
	check(OverlayWidget);
	OverlayWidget->AddToViewport();
}
