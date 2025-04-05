// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CustomAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Character/CustomHealthComponent.h"
#include "Misc/DataValidation.h"
#include "Perception/AIPerceptionComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomAIController)

ACustomAIController::ACustomAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AI Perception Component");
}

void ACustomAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	check(InPawn);
	
	// AI should only exist on Server
	if (!HasAuthority()) return;

	ensureAlwaysMsgf(MainBehaviorTree, TEXT("Please assign MainBehaviorTree in CustomAIController"));
	RunBehaviorTree(MainBehaviorTree);
	BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(GetBrainComponent());

	if (UCustomHealthComponent* HealthComp = UCustomHealthComponent::FindHealthComponent(InPawn))
	{
		HealthComp->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	}
}

FGenericTeamId ACustomAIController::GetGenericTeamId() const
{
	// Override this if needed. This is just assuming AI Controller is enemy, and Player team is 0
	// FGenericTeamId(255) is neutral (no team)
	return FGenericTeamId(1);
}

void ACustomAIController::OnDeathStarted(AActor* OwningActor, const AActor* DeathInstigator)
{
	if (BrainComponent)
	{
		BrainComponent->StopLogic("Death Started");
	}
}

#if WITH_EDITOR
EDataValidationResult ACustomAIController::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	
	// Check if the behavior tree is valid
	if (!MainBehaviorTree)
	{
		Result = EDataValidationResult::Invalid;
		Context.AddError(FText::FromString("Please assign Main Behavior Tree!"));
	}

	return Result;
}
#endif