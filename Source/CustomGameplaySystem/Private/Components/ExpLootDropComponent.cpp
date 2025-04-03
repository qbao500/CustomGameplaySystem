// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/ExpLootDropComponent.h"
#include "Character/CustomHealthComponent.h"
#include "Components/LevelExpComponent.h"
#include "DataAssets/ExpLootRewardDataAsset.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "Misc/DataValidation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ExpLootDropComponent)

UExpLootDropComponent::UExpLootDropComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	SetIsReplicatedByDefault(true);
}

void UExpLootDropComponent::BeginPlay()
{
	Super::BeginPlay();

	check(ExpLootDataAsset);

	if (!HasAuthority() || !bAutoRewardWhenOwnerDies) return;

	HealthComponent = GetOwner()->FindComponentByClass<UCustomHealthComponent>();
	if (!HealthComponent.Get()) return;

	HealthComponent->OnDeathStarted.AddDynamic(this, &UExpLootDropComponent::OnDeathStarted);
}

void UExpLootDropComponent::Server_TriggerReward_Implementation(const AActor* RewardedActor)
{
	Multicast_TriggerReward(RewardedActor);
}

void UExpLootDropComponent::RewardXP(const AActor* RewardedActor)
{
	ULevelExpComponent* TargetLevelComp = ULevelExpComponent::FindLevelExpComponent(RewardedActor);
	if (!TargetLevelComp) return;

	int32 OwnerLevel = 1;
	if (const ULevelExpComponent* OwnerLevelComp = ULevelExpComponent::FindLevelExpComponent(GetOwner()))
	{
		OwnerLevel = OwnerLevelComp->GetLevel();
	}

	TargetLevelComp->AddExp(ExpLootDataAsset->GetRewardXP(OwnerLevel));
}

void UExpLootDropComponent::RewardLoot(const AActor* RewardedActor)
{
	
}

void UExpLootDropComponent::OnDeathStarted(AActor* OwningActor, const AActor* DeathInstigator)
{
	Server_TriggerReward(DeathInstigator);
}

void UExpLootDropComponent::Multicast_TriggerReward_Implementation(const AActor* RewardedActor)
{
	RewardLoot(RewardedActor);
	RewardXP(RewardedActor);
}

bool UExpLootDropComponent::HasAuthority() const
{
	check(GetOwner());
	return GetOwner()->HasAuthority();
}


#if WITH_EDITOR
EDataValidationResult UExpLootDropComponent::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult ValidationResult = Super::IsDataValid(Context);

	if (!ExpLootDataAsset)
	{
		ValidationResult = EDataValidationResult::Invalid;
		Context.AddError(FText::FromString("ExpLootDataAsset is not set!"));
	}

	return ValidationResult;
}
#endif