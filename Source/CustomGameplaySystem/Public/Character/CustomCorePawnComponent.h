// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomPawnComponent_Base.h"
#include "CustomCorePawnComponent.generated.h"

class UCustomAbilitySystemComponent;

DECLARE_DYNAMIC_DELEGATE_OneParam(FAbilityComponentInitialized, UCustomAbilitySystemComponent*, AbilitySystemComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityComponentInitializedMulticast, UCustomAbilitySystemComponent*, AbilitySystemComponent);

/**
 * Component that adds functionality to all Pawn classes so it can be used for characters/vehicles/etc.
 * Responsible for init Ability System Component, which should be declared in PlayerState for non-NPC players.
 */
UCLASS(Within = "Pawn")
class CUSTOMGAMEPLAYSYSTEM_API UCustomCorePawnComponent : public UCustomPawnComponent_Base
{
	GENERATED_BODY()

public:

	UCustomCorePawnComponent(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure)
	static UCustomCorePawnComponent* FindCorePawnComponent(const AActor* Actor);
	static UCustomCorePawnComponent* FindCorePawnComponent(const APawn* Pawn);

	UFUNCTION(BlueprintPure, Category = "Custom|Pawn")
	UCustomAbilitySystemComponent* GetCustomAbilitySystemComponent() const { return AbilitySystemComponent; }

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	//~ End IGameFrameworkInitStateInterface interface

	//~ Begin UCustomPawnComponent_Base interface
	virtual void HandleControllerChanged() override;
	//~ End UCustomPawnComponent_Base interface

	/** Should be called by the owning pawn to become the avatar of the ability system. */
	void InitializeAbilitySystem(UCustomAbilitySystemComponent* InASC, AActor* InOwnerActor);

	/** Should be called by the owning pawn to remove itself as the avatar of the ability system. */
	void UninitializeAbilitySystem();

	/** Register with the OnAbilitySystemInitialized delegate and broadcast if our pawn has been registered with the ability system component */
	UFUNCTION(BlueprintCallable)
	void OnAbilitySystemInitialized_RegisterAndCall(const FAbilityComponentInitialized& Delegate, const bool bOnceOnly = true);

	/** Register with the OnAbilitySystemUninitialized delegate fired when our pawn is removed as the ability system's avatar actor */
	void OnAbilitySystemUninitialized_Register(const FSimpleMulticastDelegate::FDelegate& Delegate);

protected:

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Delegate fired when our pawn becomes the ability system's avatar actor */
	FAbilityComponentInitializedMulticast OnAbilitySystemInitialized;

	/** Delegate fired when our pawn is removed as the ability system's avatar actor */
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;
	
	/** Pointer to the ability system component that is cached for convenience. */
	UPROPERTY(Transient)
	TObjectPtr<UCustomAbilitySystemComponent> AbilitySystemComponent;

private:

	UPROPERTY()
	TSet<const UObject*> ListenOnceObjects;

	void BroadcastAbilitySystemInitialized();
};
