// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimInstanceProxy.h"
#include "CustomAnimInstance.generated.h"

USTRUCT()
struct FCustomAnimInstanceProxy : public FAnimInstanceProxy
{
	GENERATED_BODY()

protected:

	virtual void InitializeObjects(UAnimInstance* InAnimInstance) override;
	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;
	virtual void Update(float DeltaSeconds) override;

public:

	UPROPERTY(Transient)
	TObjectPtr<APawn> PawnOwner;
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> CharacterOwner;
	UPROPERTY(Transient)
	TObjectPtr<UPawnMovementComponent> MovementComponent;
};

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UCustomAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	
	virtual void NativeBeginPlay() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(Transient)
	FCustomAnimInstanceProxy Proxy;
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;
	virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy) override {}

public:
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	FVector Velocity = FVector(0);
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	FVector LastVelocity = FVector(0);
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	bool bIsFalling = false;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	float MoveSpeed = 0.0f;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	float MoveDirection = 0.0f;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	bool bIsMoving = false;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	float MoveSpeedIncludingZ = 0.0f;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	bool bIsAlive = true;

	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer GetCurrentAnimTags() const;

	// Only work for Montage. Require AnimMetaData to implement AnimMetaDataInterface, and override the related function
	UFUNCTION(BlueprintPure)
	static FGameplayTagContainer GetAnimTagsFromMetaData(const UAnimInstance* AnimInstance);

protected:

	// AnimMetaData needs to implement AnimMetaDataInterface
	UPROPERTY(EditDefaultsOnly)
	bool bTriggerMetaDataEventOnMontageStarted = false;

private:

	UFUNCTION()
	void OnMontageStartedMetaData(UAnimMontage* Montage);

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "Debug Editor Only")
	bool bDebugAnimTags = false;
#endif
};
