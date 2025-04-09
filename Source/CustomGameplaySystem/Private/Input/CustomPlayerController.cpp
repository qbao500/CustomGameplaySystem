// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/CustomPlayerController.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "EnhancedInputSubsystems.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "GameFramework/PlayerState.h"
#include "GAS/Components/CustomAbilitySystemComponent.h"
#include "Input/CustomInputComponent.h"
#include "Misc/DataValidation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomPlayerController)

void ACustomPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	// Cache CustomASC for Server
	CustomASC = CastChecked<UCustomAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState));
}

void ACustomPlayerController::OnUnPossess()
{
	// Make sure the pawn that is being unpossessed doesn't remain our ASC's avatar actor
	if (const APawn* PawnBeingUnpossessed = GetPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
		{
			if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
			{
				ASC->SetAvatarActor(nullptr);
			}
		}
	}

	CustomASC = nullptr;
	
	Super::OnUnPossess();
}

void ACustomPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Cache CustomASC for Client
	CustomASC = CastChecked<UCustomAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState));
}

void ACustomPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	const ULocalPlayer* LocalPlayer = GetLocalPlayer();
	check(LocalPlayer);
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);
	Subsystem->ClearAllMappings();

	FModifyContextOptions Options;
	Options.bIgnoreAllPressedKeysUntilRelease = false;

	int32 Priority = 0;
	for (const UInputMappingContext* Context : InputMappingContexts)
	{
		// Add Input Mapping Context
		Subsystem->AddMappingContext(Context, Priority, Options);
		++Priority;
	}

	UCustomInputComponent* CustomInputComponent = Cast<UCustomInputComponent>(InputComponent);
	if (ensureMsgf(CustomInputComponent,
		TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. "
		"Change the input component to UCustomInputComponent or a subclass of it.")))
	{
		// Bind Ability Actions
		TArray<uint32> Handles = CustomInputComponent->BindAbilityActions(InputConfig, this,
			&ThisClass::AbilityInputTagPressed,	&ThisClass::AbilityInputTagReleased);
	}
}

void ACustomPlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
}

void ACustomPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (CustomASC)
	{
		CustomASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}
	
	Super::PostProcessInput(DeltaTime, bGamePaused);
}

UCustomAbilitySystemComponent* ACustomPlayerController::GetCustomASC() const
{
	return CustomASC;
}

void ACustomPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (!CustomASC) return;
	CustomASC->AbilityInputTagPressed(InputTag);
}

void ACustomPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!CustomASC) return;
	CustomASC->AbilityInputTagReleased(InputTag);
}

#if WITH_EDITOR
EDataValidationResult ACustomPlayerController::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!InputConfig)
	{
		Result = EDataValidationResult::Invalid;
		Context.AddError(FText::FromString("InputConfig is not set!"));
	}

	if (InputMappingContexts.Num() <= 0)
	{
		Result = EDataValidationResult::Invalid;
		Context.AddError(FText::FromString("Please add at least one InputMappingContext!"));
	}
	else
	{
		for (int32 Index = 0; Index < InputMappingContexts.Num(); ++Index)
		{
			const UInputMappingContext* MappingContext = InputMappingContexts[Index];
			if (!MappingContext)
			{
				Result = EDataValidationResult::Invalid;
				FString ErrorMessage = FString::Printf(TEXT("InputMappingContext[%d] is not set!"), Index);
				Context.AddError(FText::FromString(ErrorMessage));
			}
		}
	}

	return Result;
}
#endif