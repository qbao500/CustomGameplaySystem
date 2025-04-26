// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "CrowdControlComponent.generated.h"

class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FCrowdControlInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag TagType = FGameplayTag();
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Causer;
	UPROPERTY(BlueprintReadWrite)
	float Duration = 0.0f;

	FTimerHandle ExpireHandle;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CUSTOMGAMEPLAYSYSTEM_API UCrowdControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UCrowdControlComponent();
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilityComp;
};
