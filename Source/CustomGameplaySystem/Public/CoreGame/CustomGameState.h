// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemInterface.h"
#include "ModularGameState.h"
#include "CustomGameState.generated.h"

class UCustomAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API ACustomGameState : public AModularGameStateBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	ACustomGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//~ Begin AActor interface
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;
	//~ End AActor interface

	//~ Begin IAbilitySystemInterface interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface interface

	UFUNCTION(BlueprintPure)
	UCustomAbilitySystemComponent* GetCustomAbilitySystemComponent() const;

	// Gets the server's FPS, replicated to clients
	float GetServerFPS() const;
	
protected:

	UPROPERTY(Replicated)
	float ServerFPS = 0.0f;

private:

	// Mostly used by CustomGamePhaseSubsystem to managing GamePhase abilities
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCustomAbilitySystemComponent> AbilitySystemComponent;
};
