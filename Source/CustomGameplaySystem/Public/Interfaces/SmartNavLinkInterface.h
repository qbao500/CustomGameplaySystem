// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"
#include "SmartNavLinkInterface.generated.h"

class ANavLinkProxy;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USmartNavLinkInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CUSTOMGAMEPLAYSYSTEM_API ISmartNavLinkInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void JumpToDestination(ANavLinkProxy* NavLink, const FVector& Destination, const float Arc = 0.5f, const float ForceMultiplier = 1.0f);
};
