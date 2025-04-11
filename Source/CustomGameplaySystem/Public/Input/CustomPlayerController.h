// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ModularPlayerController.h"
#include "CustomPlayerController.generated.h"

class UInputMappingContext;
class UCustomInputConfig;
class UCustomAbilitySystemComponent;
struct FInputActionValue;

/**
 * Main Player Controller of this CustomGameplaySystem plugin.
 * Make sure to use CustomInputComponent for input handling, set in Project Settings.
 * Override InitializePlayerInputFromConfig to bind more input (InputConfig is from PawnData).
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
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void AddCheats(bool bForce) override;
	//~ End APlayerController interface

	virtual void InitializePlayerInputFromConfig(const UCustomInputConfig* InputConfig);

protected:

	UPROPERTY(EditDefaultsOnly)
	TArray<UInputMappingContext*> InputMappingContexts;

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
