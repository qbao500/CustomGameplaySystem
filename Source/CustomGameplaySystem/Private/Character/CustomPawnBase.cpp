// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CustomPawnBase.h"

#include "CustomGameplayTags.h"
#include "Character/CustomCorePawnComponent.h"
#include "Character/CustomHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PlayerState.h"
#include "GAS/Components/CustomAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomPawnBase)

ACustomPawnBase::ACustomPawnBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// Character rotation only changes in Yaw, to prevent the capsule from changing orientation.
	// Ask the Controller for the full rotation if desired (ie for aiming).
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	// Capsule component
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CollisionCylinder");
	CapsuleComponent->InitCapsuleSize(34.0f, 88.0f);
	CapsuleComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	CapsuleComponent->CanCharacterStepUpOn = ECB_No;
	CapsuleComponent->SetShouldUpdatePhysicsVolume(true);
	CapsuleComponent->SetCanEverAffectNavigation(false);
	CapsuleComponent->bDynamicObstacle = true;
	SetRootComponent(CapsuleComponent);

	// Movement component
	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement Component"));
	MovementComponent->UpdatedComponent = CapsuleComponent;

	// For GAS
	CorePawnComponent = CreateDefaultSubobject<UCustomCorePawnComponent>("Core Pawn Component");
	HealthComponent = CreateDefaultSubobject<UCustomHealthComponent>("Health Component");
}

void ACustomPawnBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!IsValid(this)) return;

	if (MovementComponent && CapsuleComponent)
	{
		MovementComponent->UpdateNavAgent(*CapsuleComponent);
	}

	FAbilityComponentInitialized Delegate;
	Delegate.BindDynamic(this, &ThisClass::OnAbilitySystemInitialized);
	CorePawnComponent->OnAbilitySystemInitialized_RegisterAndCall(Delegate, true);
	CorePawnComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));
}

void ACustomPawnBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACustomPawnBase::Reset()
{
	DisableMovementAndCollision();

	K2_OnReset();

	UninitAndDestroy();
}

// Called by Server or in Standalone game
void ACustomPawnBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	CorePawnComponent->HandleControllerChanged();
}

void ACustomPawnBase::UnPossessed()
{
	Super::UnPossessed();
	
	CorePawnComponent->HandleControllerChanged();
}

void ACustomPawnBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	CorePawnComponent->SetupPlayerInputComponent();
}

void ACustomPawnBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	CorePawnComponent->HandleControllerChanged();
}

void ACustomPawnBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	CorePawnComponent->HandlePlayerStateReplicated();
}

void ACustomPawnBase::FaceRotation(FRotator NewControlRotation, float DeltaTime)
{
	// Only if we actually are going to use any component of rotation.
	if (bUseControllerRotationPitch || bUseControllerRotationYaw || bUseControllerRotationRoll)
	{
		const FRotator CurrentRotation = GetActorRotation();

		NewControlRotation = FMath::QInterpTo(FQuat(CurrentRotation), FQuat(NewControlRotation), DeltaTime, SmoothRotationSpeed).Rotator();

		if (!bUseControllerRotationPitch)
		{
			NewControlRotation.Pitch = CurrentRotation.Pitch;
		}

		if (!bUseControllerRotationYaw)
		{
			NewControlRotation.Yaw = CurrentRotation.Yaw;
		}

		if (!bUseControllerRotationRoll)
		{
			NewControlRotation.Roll = CurrentRotation.Roll;
		}

#if ENABLE_NAN_DIAGNOSTIC
		if (NewControlRotation.ContainsNaN())
		{
			logOrEnsureNanError(TEXT("APawn::FaceRotation about to apply NaN-containing rotation to actor! New:(%s), Current:(%s)"), *NewControlRotation.ToString(), *CurrentRotation.ToString());
		}
#endif

		SetActorRotation(NewControlRotation);
	}
}

UPawnMovementComponent* ACustomPawnBase::GetMovementComponent() const
{
	return MovementComponent;
}

UAbilitySystemComponent* ACustomPawnBase::GetAbilitySystemComponent() const
{
	if (CorePawnComponent == nullptr)
	{
		return nullptr;
	}

	return CorePawnComponent->GetCustomAbilitySystemComponent();
}

UCustomAbilitySystemComponent* ACustomPawnBase::GetCustomAbilitySystemComponent() const
{
	return Cast<UCustomAbilitySystemComponent>(GetAbilitySystemComponent());
}

void ACustomPawnBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool ACustomPawnBase::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		return ASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool ACustomPawnBase::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		return ASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool ACustomPawnBase::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		return ASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

UCapsuleComponent* ACustomPawnBase::GetCapsule_Implementation() const
{
	return CapsuleComponent;
}

FVector ACustomPawnBase::GetFootLocation_Implementation() const
{
	return GetActorLocation() - CapsuleComponent->GetScaledCapsuleHalfHeight();
}

bool ACustomPawnBase::IsAlive_Implementation()
{
	return true;
	//return HealthComponent->GetHealth() > 0.0f;
}

bool ACustomPawnBase::IsOnAir_Implementation() const
{
	// Should override in child class if needed
	return false;
}

FGenericTeamId ACustomPawnBase::GetGenericTeamId() const
{
	if (const IGenericTeamAgentInterface* TeamAI = Cast<IGenericTeamAgentInterface>(GetController()))
	{
		return TeamAI->GetGenericTeamId();
	}
	
	return FGenericTeamId(0);
}

ULevelExpComponent* ACustomPawnBase::GetLevelExpComponent_Implementation() const
{
	if (!GetPlayerState()) return nullptr;
	
	return Execute_GetLevelExpComponent(GetPlayerState());
}

int32 ACustomPawnBase::GetCurrentLevel_Implementation() const
{
	if (!GetPlayerState()) return 1;
	
	return Execute_GetCurrentLevel(GetPlayerState());
}

int32 ACustomPawnBase::GetCurrentXP_Implementation() const
{
	if (!GetPlayerState()) return 0;

	return Execute_GetCurrentXP(GetPlayerState());
}

UCapsuleComponent* ACustomPawnBase::GetCapsuleComponent() const
{
	return CapsuleComponent;
}

UFloatingPawnMovement* ACustomPawnBase::GetPawnMovement() const
{
	return MovementComponent;
}

void ACustomPawnBase::InitStartUpAbilities()
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

void ACustomPawnBase::OnAbilitySystemInitialized(UCustomAbilitySystemComponent* CustomASC)
{
	check(CustomASC);
	
	InitStartUpAbilities();

	HealthComponent->InitializeWithAbilitySystem(CustomASC);
	
	//InitializeGameplayTags();
}

void ACustomPawnBase::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeFromAbilitySystem();
}

void ACustomPawnBase::DisableMovementAndCollision() const
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UPawnMovementComponent* MoveComp = GetMovementComponent();
	check(MoveComp);
	MoveComp->StopMovementImmediately();
}

void ACustomPawnBase::UninitAndDestroy()
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

void ACustomPawnBase::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& Effect, const float Level) const
{
	if (!Effect) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC) return;

	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Effect, Level, ContextHandle);

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}