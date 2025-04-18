// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Feedback/NumberPopUp/CustomNumberPopComponent_Base.h"
#include "CustomNumberPopComponent_Niagara.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

/**
 * Initially made this to work with Lyra's damage pop Niagara.
 * So this serves as an example, you subclass the Base one and customize your own.
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UCustomNumberPopComponent_Niagara : public UCustomNumberPopComponent_Base
{
	GENERATED_BODY()

public:

	virtual void AddNumberPop(const FNumberPopRequest& NewRequest) override;

protected:

	// Name of the Niagara Array to set the Damage information
	UPROPERTY(EditDefaultsOnly, Category = "NumberPopUp")
	FName NiagaraParameterName = NAME_None;

	// Niagara System used to display the damages
	UPROPERTY(EditDefaultsOnly, Category = "NumberPopUp")
	TObjectPtr<UNiagaraSystem> NumberNiagaraSystem;
	
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> NumberNiagaraComp;
};
