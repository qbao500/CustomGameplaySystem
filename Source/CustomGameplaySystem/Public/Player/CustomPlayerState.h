// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/LevelExpInterface.h"
#include "ModularPlayerState.h"
#include "CustomPlayerState.generated.h"

class ULevelExpComponent;
class UCustomAbilitySystemComponent;
class UAttributeSet;
class UAbilitySystemComponent;
class UHealthAttributeSet;

/**
 * 
 */
UCLASS(Abstract)
class CUSTOMGAMEPLAYSYSTEM_API ACustomPlayerState : public AModularPlayerState, public IAbilitySystemInterface, public ILevelExpInterface
{
	GENERATED_BODY()

public:

	ACustomPlayerState();

	//~ Begin AActor interface
	virtual void PostInitializeComponents() override;
	//~ End AActor interface

	//~ Begin APlayerState interface
	virtual void ClientInitialize(AController* C) override;
	//~ End APlayerState interface

	//~ Begin IAbilitySystemInterface interface
	UFUNCTION(BlueprintCallable, Category = "Custom|PlayerState")
	UCustomAbilitySystemComponent* GetCustomAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface interface

	//~ Begin ILevelExpInterface interface
	virtual ULevelExpComponent* GetLevelExpComponent_Implementation() const override;
	virtual int32 GetCurrentLevel_Implementation() const override;
	virtual int32 GetCurrentXP_Implementation() const override;
	//~ End ILevelExpInterface interface

	// TODO Make a separate library for these
	UFUNCTION(BlueprintPure)
	static AActor* FindNearestPlayer(const FVector& CheckingLocation, const TArray<APlayerState*>& PlayerStates,
		float& OutNearestDistance, const bool bCheckAlive = true);
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static AActor* FindNearestPlayerFromGameState(const UObject* WorldContextObject, const FVector& CheckingLocation,
		float& OutNearestDistance, const bool bCheckAlive = true);
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static void GetAliveCharactersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors,
		const TArray<AActor*>& IgnoreActors, const float Radius, const FVector& SphereOrigin);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCustomAbilitySystemComponent> AbilitySystemComponent;

	// Most of game genres have Health, so adding here.
	// CustomHealthComponent is also using this.
	UPROPERTY()
	TObjectPtr<UHealthAttributeSet> HealthSet;

	// Most of game genres also have Level.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<ULevelExpComponent> LevelExpComponent;
};