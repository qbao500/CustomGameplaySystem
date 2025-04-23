// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonGameInstance.h"
#include "CustomGameInstance.generated.h"

class ACustomPlayerController;
/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UCustomGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()

public:

	//~ Begin UGameInstance interface
	virtual void Init() override;
	virtual void Shutdown() override;
	//~ End UGameInstance interface

	ACustomPlayerController* GetPrimaryPlayerController() const;
};
