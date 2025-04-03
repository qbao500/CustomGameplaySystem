// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributes/CustomAttributeSet_Base.h"
#include "AgilityAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UAgilityAttributeSet : public UCustomAttributeSet_Base
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MoveSpeedMulti = 1.0f;
	ATTRIBUTE_ACCESSORS(UAgilityAttributeSet, MoveSpeedMulti);

	virtual void ClampAttributeOnChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
};
