// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "GenericTeamAgentInterface.h"
#include "ModularPawn.h"
#include "Interfaces/CombatInterface.h"
#include "Interfaces/LevelExpInterface.h"
#include "CustomPawnBase.generated.h"

class UCustomFloatingPawnMovement;
class UCustomHealthComponent;
class UGameplayAbility;
class UCustomCorePawnComponent;
class UGameplayEffect;
class UCustomAbilitySystemComponent;

/**
 * 
 */
UCLASS(Abstract)
class CUSTOMGAMEPLAYSYSTEM_API ACustomPawnBase : public AModularPawn, public IAbilitySystemInterface,
	public IGameplayTagAssetInterface, public ICombatInterface, public IGenericTeamAgentInterface, public ILevelExpInterface
{
	GENERATED_BODY()

public:

	ACustomPawnBase();

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
	virtual void FaceRotation(FRotator NewControlRotation, float DeltaTime) override;
	virtual UPawnMovementComponent* GetMovementComponent() const override;
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

	UFUNCTION(BlueprintPure)
	UCapsuleComponent* GetCapsuleComponent() const;
	UFUNCTION(BlueprintPure)
	UCustomFloatingPawnMovement* GetPawnMovement() const;

	UFUNCTION(BlueprintCallable, Category = "Launch")
	void LaunchPawn(const FVector& LaunchVelocity, const bool bXYOverride = true, const bool bZOverride = true);
	UFUNCTION(BlueprintImplementableEvent, Category = "Launch")
	void OnLaunched(const FVector& LaunchVelocity, const bool bXYOverride, const bool bZOverride);

	// Used when rotate to Controller's rotation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")
	float SmoothRotationSpeed = 10.0f;

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

	void DisableMovementAndCollision() const;
	void UninitAndDestroy();

	void ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& Effect, const float Level = 1.0f) const;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCustomFloatingPawnMovement> MovementComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> CapsuleComponent;
};
