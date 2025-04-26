// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "PrintLogFunctionLibrary.generated.h"

#define PLFL UPrintLogFunctionLibrary

/**
 *  Print and debug, mostly for C++
 */

UCLASS()
class CUSTOMGAMEPLAYLIBRARY_API UPrintLogFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	static void TimestampEditor(const char* InMessage, AActor* InContext, FColor InColor = FColor::Orange, bool InLog = true);
	static void PrintEditor(const char* InMessage, const AActor* InContext, FColor InColor = FColor::Orange, bool InLog = true);
	static void EditorString(const FString& InString, FColor InColor = FColor::Orange, bool InLog = true);
	static void EditorFloat(float InValue, const char* InMessage, FColor InColor = FColor::Orange, bool InLog = true);
	static void EditorFloats(const TArray<float>& InValues, const char* InMessage, FColor InColor = FColor::Orange, bool InLog = true);
	static void PrintString(const FString& Text, float DisplayTime = 2, FColor Color = FColor::Cyan, int32 Slot = -1);
	static void ConsoleLog(const FString& Text);
	static void PrintLog(const FString& Text, float DisplayTime = 5, FColor Color = FColor::Orange);
	static void PrintWarning(const FString& Text, float DisplayTime = 10, FColor Color = FColor::Yellow);
	static void PrintError(const FString& Text, float DisplayTime = 30, FColor Color = FColor::Red);
	static void PrintPlatform(const FString& InString, const FString& InPlatform);
	static void DebugValue(const FName& InName, const FString& InAppendix);
	static void DebugValue(const FText& InText, const FString& InAppendix);
	static void DebugValue(const FString& InString, const FString& InAppendix = FString());
	static void DebugValue(int32 InValue, const FString& InAppendix = FString());
	static void DebugValue(float InValue, const FString& InAppendix = FString());
	static void DebugValue(double InValue, const FString& InAppendix = FString());
	static void DebugValue(bool InValue, const FString& InAppendix = FString());
	static void ConsoleValue(float InValue, FString InAppendix);
	static void ConsoleValue(FString InString, FString InAppendix);

	template<typename EnumType>
	static FString EnumToString(const EnumType EnumeratorValue)
	{
		return UEnum::GetDisplayValueAsText(EnumeratorValue).ToString();
	}

	template<typename EnumType>
	static void PrintEnum(const EnumType EnumeratorValue, FString InAppendix = FString(), float DisplayTime = 2, FColor Color = FColor::Cyan)
	{
		FString Message;
		Message += TEXT("MFL | PrintEnum: [");
		Message += FString::Printf(TEXT("%s"), *EnumToString(EnumeratorValue));
		Message += TEXT("]");
		if (InAppendix != FString())
		{
			Message += TEXT(" | ");
			Message += InAppendix;
		} 
		PrintString(Message, DisplayTime, Color);
	}

	// BP and C++
	UFUNCTION(BlueprintCallable)
	static void DebugTagContainer(const FGameplayTagContainer& TagContainer, float DisplayTime = 5, FColor Color = FColor::Cyan);
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "ContextObject"))
	static FString GetClientServerContextString(UObject* ContextObject);
};