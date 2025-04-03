// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/CustomAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "FunctionLibraries/AnimationFunctionLibrary.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Interfaces/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animation/AnimMetaData.h"
#include "Interfaces/AnimMetaDataInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomAnimInstance)

void FCustomAnimInstanceProxy::InitializeObjects(UAnimInstance* InAnimInstance)
{
	FAnimInstanceProxy::InitializeObjects(InAnimInstance);

	PawnOwner = InAnimInstance->TryGetPawnOwner();
	if (!PawnOwner) return;

	CharacterOwner = Cast<ACharacter>(PawnOwner);
	MovementComponent = PawnOwner->GetMovementComponent();
}

void FCustomAnimInstanceProxy::PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds)
{
	FAnimInstanceProxy::PreUpdate(InAnimInstance, DeltaSeconds);
}

void FCustomAnimInstanceProxy::Update(float DeltaSeconds)
{
	FAnimInstanceProxy::Update(DeltaSeconds);
}

void UCustomAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	if (bTriggerMetaDataEventOnMontageStarted)
	{
		OnMontageStarted.AddDynamic(this, &ThisClass::OnMontageStartedMetaData);
	}
}

void UCustomAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (!Proxy.PawnOwner) return;

	LastVelocity = Velocity;
	Velocity = Proxy.MovementComponent->Velocity;
	bIsFalling = Proxy.MovementComponent->IsFalling();
	
	MoveSpeed = Velocity.Size2D();
	MoveDirection = UKismetAnimationLibrary::CalculateDirection(Velocity, Proxy.PawnOwner->GetActorRotation());
	bIsMoving = MoveSpeed > 1.0f;
	MoveSpeedIncludingZ = Velocity.Size();

	if (UKismetSystemLibrary::DoesImplementInterface(Proxy.PawnOwner, UCombatInterface::StaticClass()))
	{
		bIsAlive = ICombatInterface::Execute_IsAlive(Proxy.PawnOwner);
	}

	// Debug
#if WITH_EDITOR
	if (bDebugAnimTags)
	{
		FGameplayTagContainer Tags = GetCurrentAnimTags();
		int32 Slot = 453;
		for (const FGameplayTag& Tag : Tags)
		{
			PLFL::PrintString(Tag.ToString(), 0, FColor::Cyan, Slot);
			Slot++;
		}
	}
#endif
}

FAnimInstanceProxy* UCustomAnimInstance::CreateAnimInstanceProxy()
{
	return &Proxy;
}

FGameplayTagContainer UCustomAnimInstance::GetCurrentAnimTags() const
{
	return GetAnimTagsFromMetaData(this);
}

FGameplayTagContainer UCustomAnimInstance::GetAnimTagsFromMetaData(const UAnimInstance* AnimInstance)
{
	FGameplayTagContainer Tags;

	if (!AnimInstance) return Tags;

	for (const FAnimMontageInstance* MontageInst : AnimInstance->MontageInstances)
	{
		if (!MontageInst || !MontageInst->IsActive()) continue;

		for (const UAnimMetaData* Data : ANIM_FL::GetAnimMetaData(MontageInst->Montage))
		{
			//PLFL::PrintWarning(MontageInst->Montage.GetName());
			if (UKismetSystemLibrary::DoesImplementInterface(Data, UAnimMetaDataInterface::StaticClass()))
			{
				Tags.AppendTags(IAnimMetaDataInterface::Execute_GetCurrentTags(Data));
			}
		}
	}

	return Tags;
}

void UCustomAnimInstance::OnMontageStartedMetaData(UAnimMontage* Montage)
{
	if (!Montage) return;
	
	for (UAnimMetaData* Data : ANIM_FL::GetAnimMetaData(Montage))
	{
		//PLFL::PrintString(GetNameSafe(Data));

		if (UKismetSystemLibrary::DoesImplementInterface(Data, UAnimMetaDataInterface::StaticClass()))
		{
			IAnimMetaDataInterface::Execute_OnMontageStarted(Data, Montage, GetOwningComponent());
		}
	}
}

