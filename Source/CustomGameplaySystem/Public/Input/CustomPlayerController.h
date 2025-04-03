// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ModularPlayerController.h"
#include "CustomPlayerController.generated.h"

class UCustomInputConfig;
class UCustomAbilitySystemComponent;

/**
 * Main Player Controller of this CustomGameplaySystem plugin.
 * Make sure to use CustomInputComponent for input handling, set in Project Settings.
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API ACustomPlayerController : public AModularPlayerController
{
	GENERATED_BODY()

public:

	//~ Begin AController interface
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void OnRep_PlayerState() override;
	//~ End AController interface
	
	//~ Begin APlayerController interface
	virtual void SetupInputComponent() override;
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~ End APlayerController interface

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCustomInputConfig> InputConfig;

	UFUNCTION(BlueprintPure)
	UCustomAbilitySystemComponent* GetCustomASC() const;

	virtual void AbilityInputTagPressed(FGameplayTag InputTag);
	virtual void AbilityInputTagReleased(FGameplayTag InputTag);

private:

	UPROPERTY()
	TObjectPtr<UCustomAbilitySystemComponent> CustomASC;

public:

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
