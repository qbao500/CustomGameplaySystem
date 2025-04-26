// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagFunctionLibrary.generated.h"

#define TAG_FL UGameplayTagFunctionLibrary

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYLIBRARY_API UGameplayTagFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, meta = (DisplayName = "To String (GameplayTag)", CompactNodeTitle = "->", BlueprintAutocast, AutoCreateRefTerm = "GameplayTag"), Category = "Utilities|String")
	static FString Conv_GameplayTagToString(const FGameplayTag& GameplayTag);

	UFUNCTION(BlueprintPure, meta = (AutoCreateRefTerm = "InGameplayTag"))
	static TArray<FString> ParseGameplayTagToStrings(const FGameplayTag& InGameplayTag);

	UFUNCTION(BlueprintPure, meta = (AutoCreateRefTerm = "InGameplayTag"))
	static FString GetLastStringOfGameplayTag(const FGameplayTag& InGameplayTag, const bool bAddSpaceBetween = false);

	UFUNCTION(BlueprintPure, meta = (AutoCreateRefTerm = "InGameplayTag"))
	static FText GetLastTextOfGameplayTag(const FGameplayTag& InGameplayTag, const bool bAddSpaceBetween = false);

	UFUNCTION(BlueprintPure, meta = (AutoCreateRefTerm = "InGameplayTag"))
	static FString MakeStringFromGameplayTag(const FGameplayTag& InGameplayTag, const FString& Separator = ".", int32 StartHierarchyLevel = 0);

	UFUNCTION(BlueprintPure, meta = (AutoCreateRefTerm = "GameplayTagContainer"))
	static const FGameplayTag& GetFirstGameplayTagFromContainer(const FGameplayTagContainer& GameplayTagContainer);

	static FGameplayTag MakeTag(const FName& TagName);
	static FGameplayTagContainer MakeTagContainer(const FName& Name);
	static FGameplayTagContainer MakeTagContainer(const TArray<FName>& Names);
	
	static FString AddSpacesToCamelCase(const FString& Input);
};
