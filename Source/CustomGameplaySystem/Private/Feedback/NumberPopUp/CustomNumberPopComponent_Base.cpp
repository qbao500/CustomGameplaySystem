// Fill out your copyright notice in the Description page of Project Settings.


#include "Feedback/NumberPopUp/CustomNumberPopComponent_Base.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomNumberPopComponent_Base)

UCustomNumberPopComponent_Base::UCustomNumberPopComponent_Base(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}
