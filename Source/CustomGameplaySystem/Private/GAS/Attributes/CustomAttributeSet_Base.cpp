// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/CustomAttributeSet_Base.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomAttributeSet_Base)

UWorld* UCustomAttributeSet_Base::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

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