// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularAIController.h"
#include "CustomAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API ACustomAIController : public AModularAIController
{
	GENERATED_BODY()

public:

	ACustomAIController();

	
	virtual void OnPossess(APawn* InPawn) override;

	virtual FGenericTeamId GetGenericTeamId() const override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

private:

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBehaviorTree> MainBehaviorTree;

	UFUNCTION()
	void OnDeathStarted(AActor* OwningActor, const AActor* DeathInstigator);
};
