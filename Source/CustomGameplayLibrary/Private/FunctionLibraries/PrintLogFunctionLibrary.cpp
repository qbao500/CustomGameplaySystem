// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/PrintLogFunctionLibrary.h"

#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PrintLogFunctionLibrary)

void UPrintLogFunctionLibrary::TimestampEditor(const char* InMessage, AActor* InContext, FColor InColor, bool InLog)
{
	if (!InContext) return ;
	const FString InText = FString(InMessage) + TEXT(" | TimeSeconds | ") + FString::SanitizeFloat(InContext->GetWorld()->GetTimeSeconds());
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.0f, InColor, InText);
	if (InLog) ConsoleLog(InText);
}

void UPrintLogFunctionLibrary::PrintEditor(const char* InMessage, const AActor* InContext, FColor InColor, bool InLog)
{
	if (!InContext) return ;
	const FString InText = FString(InMessage); 
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.0f, InColor, InText);
	if (InLog) ConsoleLog(InText);
}

void UPrintLogFunctionLibrary::EditorString(const FString& InString, FColor InColor, bool InLog)
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.0f, InColor, InString);
	if (InLog) ConsoleLog(InString);
}

void UPrintLogFunctionLibrary::EditorFloat(float InValue, const char* InMessage, FColor InColor, bool InLog)
{
	const FString InText = FString::SanitizeFloat(InValue) + TEXT(" | ") + FString(InMessage); 
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.0f, InColor, InText);
	if (InLog) ConsoleLog(InText);
}

void UPrintLogFunctionLibrary::EditorFloats(const TArray<float>& InValues, const char* InMessage, FColor InColor, bool InLog)
{
	FString OutText = FString(TEXT("EditorFloats (")) + FString::FromInt(InValues.Num()) + FString(TEXT("): "));
	bool bFirstFloat = true;
	for (float Value : InValues)
	{
		if (bFirstFloat)
		{
			bFirstFloat = false;
		}
		else
		{
			OutText += FString(TEXT(" | "));
		}
		OutText += FString::SanitizeFloat(Value); 
	}
	OutText += TEXT(" Message=( ") + FString(InMessage);
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.0f, InColor, OutText);
	if (InLog) ConsoleLog(OutText);
	
}

void UPrintLogFunctionLibrary::PrintString(const FString& Text, float DisplayTime, FColor Color, int32 Slot)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(Slot, DisplayTime, Color, Text);
	}
}

void UPrintLogFunctionLibrary::ConsoleValue(float InValue, FString InAppendix)
{
	FString Message;
	Message += TEXT("PLFL | DebugValue (FString): [");
	Message += FString::Printf(TEXT("%f"), InValue);
	Message += TEXT("]");
	if (InAppendix != FString())
	{
		Message += TEXT(" | ");
		Message += InAppendix;
	} 
	ConsoleLog(Message);
}

void UPrintLogFunctionLibrary::ConsoleValue(FString InString, FString InAppendix)
{
	FString Message;
	Message += TEXT("PLFL | DebugValue (FString): [");
	Message += InString;
	Message += TEXT("]");
	if (InAppendix != FString())
	{
		Message += TEXT(" | ");
		Message += InAppendix;
	} 
	ConsoleLog(Message);
}

void UPrintLogFunctionLibrary::DebugTagContainer(const FGameplayTagContainer& TagContainer, const float DisplayTime, const FColor Color)
{
	for (const FGameplayTag& Tag : TagContainer.GetGameplayTagArray())
	{
		PrintLog(Tag.ToString(), DisplayTime, Color);
	}
}

void UPrintLogFunctionLibrary::ConsoleLog(const FString& Text)
{
	UE_LOG(LogTemp, Log, TEXT("%s"), *Text);
}

void UPrintLogFunctionLibrary::PrintLog(const FString& Text, float DisplayTime, FColor Color)
{
	PrintString(Text, DisplayTime, Color);
	UE_LOG(LogTemp, Log, TEXT("%s"), *Text);
}

void UPrintLogFunctionLibrary::PrintWarning(const FString& Text, float DisplayTime, FColor Color)
{
	PrintString(Text, DisplayTime, Color);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Text);
}

void UPrintLogFunctionLibrary::PrintError(const FString& Text, float DisplayTime, FColor Color)
{
	PrintString(Text, DisplayTime, Color);
	UE_LOG(LogTemp, Error, TEXT("%s"), *Text);
}

void UPrintLogFunctionLibrary::PrintPlatform(const FString& InString, const FString& InPlatform)
{
	const FString CurrentPlatform = UGameplayStatics::GetPlatformName();
	UE_LOG(LogTemp, Warning, TEXT("Current platform: %s. Requesting platform: %s"), *CurrentPlatform, *InPlatform);
	if (CurrentPlatform.Equals(InPlatform))
	{
		PrintString("[" + InPlatform + "] " + InString, 10, FColor::Magenta);
	}
}

void UPrintLogFunctionLibrary::DebugValue(const FName& InName, const FString& InAppendix)
{
	FString Message;
	Message += TEXT("PLFL | DebugValue (FName): [");
	Message += InName.ToString();
	Message += TEXT("]");
	if (InAppendix != FString())
	{
		Message += TEXT(" | ");
		Message += InAppendix;
	} 
	PrintLog(Message);
}

void UPrintLogFunctionLibrary::DebugValue(const FText& InText, const FString& InAppendix)
{
	FString Message;
	Message += TEXT("PLFL | DebugValue (FName): [");
	Message += InText.ToString();
	Message += TEXT("]");
	if (InAppendix != FString())
	{
		Message += TEXT(" | ");
		Message += InAppendix;
	} 
	PrintLog(Message);
}

void UPrintLogFunctionLibrary::DebugValue(const FString& InString, const FString& InAppendix)
{
	FString Message;
	Message += TEXT("PLFL | DebugValue (FString): [");
	Message += InString;
	Message += TEXT("]");
	if (InAppendix != FString())
	{
		Message += TEXT(" | ");
		Message += InAppendix;
	} 
	PrintLog(Message);
}

void UPrintLogFunctionLibrary::DebugValue(int32 InValue, const FString& InAppendix)
{
	FString Message;
	Message += TEXT("PLFL | DebugValue (int32): [");
	Message += FString::Printf(TEXT("%d"), InValue);
	Message += TEXT("]");
	if (InAppendix != FString())
	{
		Message += TEXT(" | ");
		Message += InAppendix;
	} 
	PrintLog(Message);
}

void UPrintLogFunctionLibrary::DebugValue(float InValue, const FString& InAppendix)
{
	FString Message;
	Message += TEXT("PLFL | DebugValue (float): [");
	Message += FString::Printf(TEXT("%f"), InValue);
	Message += TEXT("]");
	if (InAppendix != FString())
	{
		Message += TEXT(" | ");
		Message += InAppendix;
	} 
	PrintLog(Message);
}

void UPrintLogFunctionLibrary::DebugValue(double InValue, const FString& InAppendix)
{
	FString Message;
	Message += TEXT("PLFL | DebugValue (float): [");
	Message += FString::Printf(TEXT("%f"), InValue);
	Message += TEXT("]");
	if (InAppendix != FString())
	{
		Message += TEXT(" | ");
		Message += InAppendix;
	} 
	PrintLog(Message);
}

void UPrintLogFunctionLibrary::DebugValue(bool InValue, const FString& InAppendix)
{
	FString Message;
	Message += TEXT("PLFL | DebugValue (bool): [");
	Message += InValue ? "True" : "False";
	Message += TEXT("]");
	if (InAppendix != FString())
	{
		Message += TEXT(" | ");
		Message += InAppendix;
	} 
	PrintLog(Message);
}

FString UPrintLogFunctionLibrary::GetClientServerContextString(UObject* ContextObject)
{
	ENetRole Role = ROLE_None;

	if (const AActor* Actor = Cast<AActor>(ContextObject))
	{
		Role = Actor->GetLocalRole();
	}
	else if (const UActorComponent* Component = Cast<UActorComponent>(ContextObject))
	{
		Role = Component->GetOwnerRole();
	}

	if (Role != ROLE_None)
	{
		const FString RoleString = (Role == ROLE_Authority) ? TEXT("Server") : TEXT("Client");
		return "(" + RoleString + ")";
	}
	else
	{
#if WITH_EDITOR
		if (GIsEditor)
		{
			extern ENGINE_API FString GPlayInEditorContextString;
			return GPlayInEditorContextString;
		}
#endif
	}

	return TEXT("[]");
}