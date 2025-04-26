// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "Components/ControllerComponent.h"
#include "CustomNumberPopComponent_Base.generated.h"

USTRUCT(BlueprintType)
struct FNumberPopRequest
{
	GENERATED_BODY()

	// The world location to create the number pop at
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|NumberPopUp")
	FVector WorldLocation = FVector(0);

	// The number to display
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|NumberPopUp")
	int32 NumberToDisplay = 0;

	// Or you could use this instead of just number
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|NumberPopUp")
	FString StringToDisplay = FString();

	// More info of the number
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|NumberPopUp")
	FGameplayTagContainer InfoTags = FGameplayTagContainer::EmptyContainer;
};

/**
 * Component to handle number (or text) pop up.
 * This need be on a Controller class.
 * Subclass this and override AddNumberPop() for customized behavior.
 */
UCLASS(Abstract)
class CUSTOMGAMEPLAYSYSTEM_API UCustomNumberPopComponent_Base : public UControllerComponent
{
	GENERATED_BODY()

public:

	UCustomNumberPopComponent_Base(const FObjectInitializer& ObjectInitializer);

	/** Adds a damage number to the damage number list for visualization */
	UFUNCTION(BlueprintCallable, Category = "Custom|NumberPopUp")
	virtual void AddNumberPop(const FNumberPopRequest& NewRequest) {}
};
