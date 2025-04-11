// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CustomPawnComponent_Base.h"
#include "CustomLocalPawnComponent.generated.h"

/**
 * Pawn Component for real (local) Player, that handles input and camera.
 * Since it's local, no replication is needed.
 * While CustomCorePawnComponent is already added to CustomPawnBase and CustomCharacterBase,
 * this component needs to be manually added to your child Pawn class, which is controlled by a Player.
 * Nevertheless, this component needs CustomCorePawnComponent to be added to the same Pawn, to work correctly.
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class CUSTOMGAMEPLAYSYSTEM_API UCustomLocalPawnComponent : public UCustomPawnComponent_Base
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure)
	static UCustomLocalPawnComponent* FindLocalPawnComponent(AActor* Actor);
	static UCustomLocalPawnComponent* FindLocalPawnComponent(const APawn* Pawn);

	/** The name of this component-implemented feature */
	static const FName NAME_ActorFeatureName;

	/** The name of the extension event sent via UGameFrameworkComponentManager when ability inputs are ready to bind */
	static const FName NAME_BindInputsNow;

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	//~ End IGameFrameworkInitStateInterface interface

protected:

	//~ Begin UActorComponent interface
	virtual void BeginPlay() override;
	//~ End UActorComponent interface

private:

	/** True when player input bindings have been applied, will never be true for non - players */
	bool bReadyToBindExtraInputs = false;
};
