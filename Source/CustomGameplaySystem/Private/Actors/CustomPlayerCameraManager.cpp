// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CustomPlayerCameraManager.h"

#include "Camera/CameraModifier.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"

UCameraModifier* ACustomPlayerCameraManager::EnableCameraModifier(const TSubclassOf<UCameraModifier> ModifierClass)
{
	if (UCameraModifier* Modifier = FindCameraModifierByClass(ModifierClass))
	{
		Modifier->EnableModifier();
		return Modifier;
	}

	return nullptr;
}

void ACustomPlayerCameraManager::DisableCameraModifier(TSubclassOf<UCameraModifier> ModifierClass, const bool bImmediate)
{
	if (UCameraModifier* Modifier = FindCameraModifierByClass(ModifierClass))
	{
		Modifier->DisableModifier(bImmediate);
	}
}