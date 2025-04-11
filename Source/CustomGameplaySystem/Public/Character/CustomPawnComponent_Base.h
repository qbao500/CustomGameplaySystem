// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "CustomPawnComponent_Base.generated.h"

/**
 * Base class for Pawn Component in this plugin, implementing IGameFrameworkInitStateInterface and overriden some functions.
 * Replicate by default. No Tick by default.
 * Please inherit from this.
 */
UCLASS(Abstract, BlueprintType)
class CUSTOMGAMEPLAYSYSTEM_API UCustomPawnComponent_Base : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:

	UCustomPawnComponent_Base(const FObjectInitializer& ObjectInitializer);

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface

	/** Returns the "Custom" Pawn Component if one exists on the specified actor. Need to Cast to right type*/
	UFUNCTION(BlueprintPure, Category = "Pawn Component", meta = (ComponentClass = "/Script/CustomGameplaySystem.CustomPawnComponent_Base", AutoCreateRefTerm = "ComponentClass"))
	static UCustomPawnComponent_Base* FindCustomPawnComponent(const AActor* Actor, const TSubclassOf<UCustomPawnComponent_Base>& ComponentClass)
	{
		return Actor ? Cast<UCustomPawnComponent_Base>(Actor->FindComponentByClass(ComponentClass)) : nullptr;
	}
	template <class T>
	static T* FindCustomPawnComponent(const AActor* Actor)
	{
		static_assert(TPointerIsConvertibleFromTo<T, UCustomPawnComponent_Base>::Value, "'T' template parameter to FindCustomPawnComponent must be derived from UCustomPawnComponent_Base");
		return Cast<T>(FindCustomPawnComponent(Actor, T::StaticClass()));
	}
	
	/** Should be called by the owning pawn when the pawn's controller changes. */
	virtual void HandleControllerChanged();

	/** Should be called by the owning pawn when the player state has been replicated. */
	virtual void HandlePlayerStateReplicated();

	/** Should be called by the owning pawn when the input component is setup. */
	virtual void SetupPlayerInputComponent();

protected:

	//~ Begin UActorComponent interface
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End UActorComponent interface

	static APawn* FindPawnFromActor(AActor* Actor);
};
