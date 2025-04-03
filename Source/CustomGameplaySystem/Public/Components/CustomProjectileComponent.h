// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CustomProjectileComponent.generated.h"

UENUM(BlueprintType)
enum class EHomingType : uint8
{
	NoHoming,
	// Default behavior of UE. Need to set HomingTargetComponent
	ToMovingTarget,
	// Need to set HomingDestination with RegisterDestinationHoming
	ToDestination,
};

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UCustomProjectileComponent : public UProjectileMovementComponent
{
	GENERATED_BODY()

public:

	friend class AProjectileActor;

	UCustomProjectileComponent();

	//~ Begin UActorComponent interface
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent interface

	//~ Begin UProjectileMovementComponent interface
	virtual FVector ComputeVelocity(FVector InitialVelocity, float DeltaTime) const override;
	virtual FVector ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const override;
	virtual FVector ComputeHomingAcceleration(const FVector& InVelocity, float DeltaTime) const override;
	virtual bool ShouldUseSubStepping() const override;
	virtual void StopSimulating(const FHitResult& HitResult) override;
	//~ End UProjectileMovementComponent interface

	virtual FVector ComputeBoomerangAcceleration(const FVector& InVelocity, float DeltaTime) const;

	UFUNCTION(BlueprintCallable)
	void RegisterSocketHoming(USceneComponent* HomingComp, const FName& BoneName);
	
	/**
	 * 
	 * @param TargetActor Implement ProjectileTargetInterface and override its functions for custom homing
	 * @param bTargetWeakPoint
	 */
	UFUNCTION(BlueprintCallable)
	void RegisterSocketHomingFromActor(AActor* TargetActor, const bool bTargetWeakPoint);

	UFUNCTION(BlueprintCallable)
	void RegisterDestinationHoming(const FVector& Destination);
	
	UFUNCTION(BlueprintCallable)
	void ClearSocketHomingInfo();

	UFUNCTION(BlueprintPure)
	bool IsHomingProjectile() const;
	UFUNCTION(BlueprintPure)
	bool IsHomingToTarget() const;
	UFUNCTION(BlueprintPure)
	bool IsHomingToDestination() const;
	UFUNCTION(BlueprintCallable)
	void SetHomingType(EHomingType InHomingType);

	UFUNCTION(BlueprintPure)
	bool IsBoomerang() const;

protected:
	
	UPROPERTY(EditDefaultsOnly,	 BlueprintReadWrite, Category = "Homing")
	bool bHoming2D = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Homing")
	bool bCancelHomingIfPassedTarget = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boomerang")
	bool bIsBoomerang = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boomerang", meta = (EditCondition = "bIsBoomerang"))
	float BoomerangForwardDeceleration = 2000.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boomerang", meta = (EditCondition = "bIsBoomerang"))
	float BoomerangReturningAcceleration = 3000.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boomerang", meta = (EditCondition = "bIsBoomerang"))
	float BoomerangVelocityToReturn = 50.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boomerang", meta = (EditCondition = "bIsBoomerang"))
	float OwnerDistanceToMagnetizeBoomerang = 300.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boomerang", meta = (EditCondition = "bIsBoomerang"))
	float OwnerDistanceToEndBoomerang = 50.0f;

private:

	bool HasPassedHomingTarget() const;

	EHomingType HomingType = EHomingType::NoHoming;
	FName HomingSocketName = FName();
	TOptional<FVector> HomingDestination;

	UPROPERTY()
	TWeakObjectPtr<AActor> BoomerangOwner;
	bool bBoomerangReturning = false;
	FSimpleDelegate OnBoomerangStartReturning;
	FSimpleDelegate OnBoomerangFinished;
	float GetDistanceToBoomerangOwner() const;
};
