// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/AgilityAttributeSet.h"

void UAgilityAttributeSet::ClampAttributeOnChange(const FGameplayAttribute& Attribute, float& NewValue) const
{	
	if (Attribute == GetMoveSpeedMultiAttribute())
	{
		NewValue = FMath::Max(0.0f, NewValue);
	}
}
