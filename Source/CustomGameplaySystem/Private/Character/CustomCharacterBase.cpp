// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CustomCharacterBase.h"

#include "CustomGameplayTags.h"
#include "Character/CustomCorePawnComponent.h"
#include "Character/CustomHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GAS/Components/CustomAbilitySystemComponent.h"
#include "FunctionLibraries/CustomHelperFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/CustomPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomCharacterBase)

ACustomCharacterBase::ACustomCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	CorePawnComponent = CreateDefaultSubobject<UCustomCorePawnComponent>("Core Pawn Component");
	HealthComponent = CreateDefaultSubobject<UCustomHealthComponent>("Health Component");

	GetMesh()->bEnableUpdateRateOptimizations = true;

	// For AI
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ACustomCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	FAbilityComponentInitialized Delegate;
	Delegate.BindDynamic(this, &ThisClass::OnAbilitySystemInitialized);
	CorePawnComponent->OnAbilitySystemInitialized_RegisterAndCall(Delegate, true);
	CorePawnComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));
}

void ACustomCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		// When playing in Dedicated Server, animations are triggered but not played on Server side, because the mesh it not rendered
		// This makes logics that depend on socket location return wrong locations on Server
		// So force it to always refresh bones on Server, for now. Probably there's a better solution.
		GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	}
}

void ACustomCharacterBase::Reset()
{
	DisableMovementAndCollision();

	K2_OnReset();

	UninitAndDestroy();
}

// Called by Server or in Standalone game
void ACustomCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	CorePawnComponent->HandleControllerChanged();
}

void ACustomCharacterBase::UnPossessed()
{
	Super::UnPossessed();
	
	CorePawnComponent->HandleControllerChanged();
}

void ACustomCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	CorePawnComponent->SetupPlayerInputComponent();
}

void ACustomCharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	CorePawnComponent->HandleControllerChanged();
}

void ACustomCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	CorePawnComponent->HandlePlayerStateReplicated();
}

UAbilitySystemComponent* ACustomCharacterBase::GetAbilitySystemComponent() const
{
	if (CorePawnComponent == nullptr)
	{
		return nullptr;
	}

	return CorePawnComponent->GetCustomAbilitySystemComponent();
}

UCustomAbilitySystemComponent* ACustomCharacterBase::GetCustomAbilitySystemComponent() const
{
	return Cast<UCustomAbilitySystemComponent>(GetAbilitySystemComponent());
}

void ACustomCharacterBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool ACustomCharacterBase::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		return ASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool ACustomCharacterBase::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		return ASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool ACustomCharacterBase::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		return ASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

UCapsuleComponent* ACustomCharacterBase::GetCapsule_Implementation() const
{
	return GetCapsuleComponent();
}

USkeletalMeshComponent* ACustomCharacterBase::GetMainMesh_Implementation() const
{
	return GetMesh();
}

FVector ACustomCharacterBase::GetFootLocation_Implementation() const
{
	return HFL::GetCharacterFootLocation(this);
}

bool ACustomCharacterBase::IsAlive_Implementation()
{
	return HealthComponent->GetHealth() > 0.0f;
}

bool ACustomCharacterBase::IsOnAir_Implementation() const
{
	return GetCharacterMovement()->IsFalling();
}

FGenericTeamId ACustomCharacterBase::GetGenericTeamId() const
{
	if (const IGenericTeamAgentInterface* TeamAI = Cast<IGenericTeamAgentInterface>(GetController()))
	{
		return TeamAI->GetGenericTeamId();
	}
	
	return FGenericTeamId(0);
}

ULevelExpComponent* ACustomCharacterBase::GetLevelExpComponent_Implementation() const
{
	if (!GetPlayerState()) return nullptr;
	
	return Execute_GetLevelExpComponent(GetPlayerState());
}

int32 ACustomCharacterBase::GetCurrentLevel_Implementation() const
{
	if (!GetPlayerState()) return 1;
	
	return Execute_GetCurrentLevel(GetPlayerState());
}

int32 ACustomCharacterBase::GetCurrentXP_Implementation() const
{
	if (!GetPlayerState()) return 0;

	return Execute_GetCurrentXP(GetPlayerState());
}

void ACustomCharacterBase::InitStartUpAbilities()
{
	// Only do on Server
	if (!HasAuthority()) return;

	for (const TSubclassOf<UGameplayAbility> Ability : StartUpAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1);
		AbilitySpec.DynamicAbilityTags.AddTag(CustomTags::Ability_Type_Active);
		GetAbilitySystemComponent()->GiveAbility(AbilitySpec);
	}

	for (const TSubclassOf<UGameplayAbility> Passive : StartUpPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Passive, 1);
		AbilitySpec.DynamicAbilityTags.AddTag(CustomTags::Ability_Type_Passive);
		GetAbilitySystemComponent()->GiveAbility(AbilitySpec);
	}
}

void ACustomCharacterBase::OnAbilitySystemInitialized(UCustomAbilitySystemComponent* CustomASC)
{
	check(CustomASC);
	
	InitStartUpAbilities();

	HealthComponent->InitializeWithAbilitySystem(CustomASC);

	//InitializeGameplayTags();
}

void ACustomCharacterBase::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeFromAbilitySystem();
}

void ACustomCharacterBase::DisableMovementAndCollision() const
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	check(MoveComp);
	MoveComp->StopMovementImmediately();
	MoveComp->DisableMovement();
}

void ACustomCharacterBase::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (const UCustomAbilitySystemComponent* CustomASC = GetCustomAbilitySystemComponent())
	{
		if (CustomASC->GetAvatarActor() == this)
		{
			CorePawnComponent->UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}

void ACustomCharacterBase::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& Effect, const float Level) const
{
	if (!Effect) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC) return;

	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Effect, Level, ContextHandle);

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

#if WITH_EDITOR
EDataValidationResult ACustomCharacterBase::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult ValidationResult = Super::IsDataValid(Context);

	if (ValidationResult != EDataValidationResult::Invalid)
	{
		TArray<const UActorComponent*> Comps;
		GetActorClassDefaultComponents(GetClass(), Comps);
		for (const UActorComponent* Comp : Comps)
		{
			if (!IsValid(Comp)) continue;
			
			ValidationResult = Comp->IsDataValid(Context);
		}
	}

	return ValidationResult;
}
#endif