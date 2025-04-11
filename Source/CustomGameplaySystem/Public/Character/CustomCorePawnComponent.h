// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomPawnComponent_Base.h"
#include "CustomCorePawnComponent.generated.h"

class UCustomPawnData;
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
	static UCustomCorePawnComponent* FindCorePawnComponent(AActor* Actor);
	static UCustomCorePawnComponent* FindCorePawnComponent(const APawn* Pawn);

	UFUNCTION(BlueprintPure, Category = "Custom|Pawn")
	UCustomAbilitySystemComponent* GetCustomAbilitySystemComponent() const { return AbilitySystemComponent; }

	/** The name of this component-implemented feature */
	static const FName NAME_ActorFeatureName;

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override;
	virtual void CheckDefaultInitialization() override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
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

	const UCustomPawnData* GetPawnData() const;

protected:

	//~ Begin UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End UActorComponent interface

	/** Delegate fired when our pawn becomes the ability system's avatar actor */
	FAbilityComponentInitializedMulticast OnAbilitySystemInitialized;

	/** Delegate fired when our pawn is removed as the ability system's avatar actor */
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;

	// Pawn Data
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_PawnData, Category = "Pawn Data")
	TObjectPtr<const UCustomPawnData> PawnData;
	UFUNCTION()
	void OnRep_PawnData();
	
	/** Pointer to the ability system component that is cached for convenience. */
	UPROPERTY(Transient)
	TObjectPtr<UCustomAbilitySystemComponent> AbilitySystemComponent;

private:

	UFUNCTION(Server, Reliable)
	void Server_GrantAbilitySets();

	UPROPERTY()
	TSet<const UObject*> ListenOnceObjects;

	void BroadcastAbilitySystemInitialized();

	bool IsAuthorityOrLocal() const;

public:

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
