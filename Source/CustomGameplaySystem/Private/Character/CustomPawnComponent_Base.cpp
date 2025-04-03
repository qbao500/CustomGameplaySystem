// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CustomPawnComponent_Base.h"

#include "CustomGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomPawnComponent_Base)

//const FName UCustomPawnComponent_Base::NAME_ActorFeatureName("PleaseGiveNameInChildClass");

UCustomPawnComponent_Base::UCustomPawnComponent_Base(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

FName UCustomPawnComponent_Base::GetFeatureName() const
{
	return NAME_ActorFeatureName;
}

void UCustomPawnComponent_Base::CheckDefaultInitialization()
{
	// Before checking our progress, try progressing any other features we might depend on
	CheckDefaultInitializationForImplementers();

	static const TArray<FGameplayTag> StateChain = {
		CustomTags::InitState_Spawned, CustomTags::InitState_DataAvailable, CustomTags::InitState_DataInitialized, CustomTags::InitState_GameplayReady };

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

void UCustomPawnComponent_Base::HandleControllerChanged()
{
	CheckDefaultInitialization();
}

void UCustomPawnComponent_Base::HandlePlayerStateReplicated()
{
	CheckDefaultInitialization();
}

void UCustomPawnComponent_Base::SetupPlayerInputComponent()
{
	CheckDefaultInitialization();
}

void UCustomPawnComponent_Base::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("[%s] on [%s] can only be added to Pawn actors."), *StaticClass()->GetFName().ToString(), *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> PawnExtensionComponents;
	Pawn->GetComponents(StaticClass(), PawnExtensionComponents);
	ensureAlwaysMsgf((PawnExtensionComponents.Num() == 1), TEXT("Only one [%s] should exist on [%s]."), *StaticClass()->GetFName().ToString(), *GetNameSafe(GetOwner()));

	// Register with the init state system early, this will only work if this is a game world
	RegisterInitStateFeature();
}

void UCustomPawnComponent_Base::BeginPlay()
{
	Super::BeginPlay();

	// Listen for changes to all features
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);
	
	// Notifies state manager that we have spawned, then try rest of default initialization
	ensure(TryToChangeInitState(CustomTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UCustomPawnComponent_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	
	Super::EndPlay(EndPlayReason);
}
