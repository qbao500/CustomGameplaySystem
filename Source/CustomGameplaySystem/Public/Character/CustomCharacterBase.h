// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "GenericTeamAgentInterface.h"
#include "ModularCharacter.h"
#include "Interfaces/CombatInterface.h"
#include "Interfaces/LevelExpInterface.h"
#include "CustomCharacterBase.generated.h"

class UCustomAbilitySystemComponent;
class UCustomHealthComponent;
class UCustomCorePawnComponent;
class UGameplayAbility;
class UGameplayEffect;
class UAttributeSet;

UCLASS(Abstract)
class CUSTOMGAMEPLAYSYSTEM_API ACustomCharacterBase : public AModularCharacter, public IAbilitySystemInterface,
	public IGameplayTagAssetInterface, public ICombatInterface, public IGenericTeamAgentInterface, public ILevelExpInterface
{
	GENERATED_BODY()

public:

	ACustomCharacterBase();

	//~ Begin AActor interface
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Reset() override;
	//~ End AActor interface
	
	//~ Begin APawn interface
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void OnRep_Controller() override;
	virtual void OnRep_PlayerState() override;
	//~ End APawn interface

	//~ Begin IAbilitySystemInterface interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UFUNCTION(BlueprintCallable)
	UCustomAbilitySystemComponent* GetCustomAbilitySystemComponent() const;
	//~ End IAbilitySystemInterface interface

	//~ Begin IGameplayTagAssetInterface interface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	//~ End IGameplayTagAssetInterface interface

	//~ Begin ICombatInterface interface
	virtual UCapsuleComponent* GetCapsule_Implementation() const override;
	virtual USkeletalMeshComponent* GetMainMesh_Implementation() const override;
	virtual FVector GetFootLocation_Implementation() const override;
	virtual bool IsAlive_Implementation() override;
	virtual bool IsOnAir_Implementation() const override;
	//~ End ICombatInterface interface

	//~ Begin IGenericTeamAgentInterface interface
	virtual FGenericTeamId GetGenericTeamId() const override;
	//~ End IGenericTeamAgentInterface interface

	// Begin ILevelExpInterface interface
	virtual ULevelExpComponent* GetLevelExpComponent_Implementation() const override;
	virtual int32 GetCurrentLevel_Implementation() const override;
	virtual int32 GetCurrentXP_Implementation() const override;
	// End ILevelExpInterface interface

protected:

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCustomCorePawnComponent> CorePawnComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCustomHealthComponent> HealthComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Abilitites")
	TArray<TSubclassOf<UGameplayAbility>> StartUpAbilities;
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Abilitites")
	TArray<TSubclassOf<UGameplayAbility>> StartUpPassiveAbilities;
	virtual void InitStartUpAbilities();

	UFUNCTION()
	virtual void OnAbilitySystemInitialized(UCustomAbilitySystemComponent* CustomASC);
	virtual void OnAbilitySystemUninitialized();

	void DisableMovementAndCollision(const bool bStopActiveMovementImmediately = true) const;
	void UninitAndDestroy();

	void ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& Effect, const float Level = 1.0f) const;

public:

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
