// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/CustomAttributeSet_Base.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomAttributeSet_Base)

void UCustomAttributeSet_Base::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttributeOnChange(Attribute, NewValue);
}

void UCustomAttributeSet_Base::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttributeOnChange(Attribute, NewValue);
}

void UCustomAttributeSet_Base::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Retrieve necessary properties from FGameplayEffectModCallbackData and cache it
	SetEffectProperties(Data, LatestEffectProps);
}

UWorld* UCustomAttributeSet_Base::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

void UCustomAttributeSet_Base::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props)
{
	/** Source = causer of this effect. Target = target of the effect (owner of this AS) */

	// Effect Context Handle
	Props.EffectContextHandle = Data.EffectSpec.GetEffectContext();

	// Source
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	if (Props.SourceASC)
	{
		Props.SourceAvatarActor = Props.SourceASC->GetAvatarActor();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (!Props.SourceController && Props.SourceAvatarActor)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		Props.SourceCharacter = Cast<ACharacter>(Props.SourceAvatarActor); 
	}

	// Target
	Props.TargetASC = Data.Target;
	if (Props.TargetASC)
	{
		Props.TargetAvatarActor = Props.TargetASC->GetAvatarActor();
		Props.TargetController = Props.TargetASC->AbilityActorInfo->PlayerController.Get();
		if (!Props.TargetController && Props.TargetAvatarActor)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.TargetAvatarActor))
			{
				Props.TargetController = Pawn->GetController();
			}
		}
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
	}
}
