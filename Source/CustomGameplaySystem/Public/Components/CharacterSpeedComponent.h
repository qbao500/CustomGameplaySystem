// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterSpeedComponent.generated.h"

class UCharacterMovementComponent;

UENUM(BlueprintType)
enum class ESpeedMode : uint8
{
	Walking,
	Jogging,
	Sprinting,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CUSTOMGAMEPLAYSYSTEM_API UCharacterSpeedComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UCharacterSpeedComponent();
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SetSpeedMode(const ESpeedMode NewSpeedMode);

private:

	UPROPERTY(EditAnywhere)
	ESpeedMode DefaultSpeedMode = ESpeedMode::Jogging;
	UPROPERTY(EditAnywhere)
	float WalkSpeed = 200;
	UPROPERTY(EditAnywhere)
	float JogSpeed = 500;
	UPROPERTY(EditAnywhere)
	float SprintSpeed = 800;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CharacterMoveComp;

	ESpeedMode CurrentSpeedMode = ESpeedMode::Walking;
};
