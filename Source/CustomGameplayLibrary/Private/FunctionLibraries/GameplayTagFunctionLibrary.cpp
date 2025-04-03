// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/GameplayTagFunctionLibrary.h"

#include "FunctionLibraries/PrintLogFunctionLibrary.h"

FString UGameplayTagFunctionLibrary::Conv_GameplayTagToString(const FGameplayTag& GameplayTag)
{
	return GameplayTag.ToString();
}

TArray<FString> UGameplayTagFunctionLibrary::ParseGameplayTagToStrings(const FGameplayTag& InGameplayTag)
{
	TArray<FString> Strings;
	InGameplayTag.ToString().ParseIntoArray(Strings, TEXT("."));
	return Strings;
}

FString UGameplayTagFunctionLibrary::GetLastStringOfGameplayTag(const FGameplayTag& InGameplayTag, const bool bAddSpaceBetween)
{
	if (!InGameplayTag.IsValid()) return FString();
	
	TArray<FString> Strings = ParseGameplayTagToStrings(InGameplayTag);
	FString Final = Strings.Last();

	if (bAddSpaceBetween)
	{
		Final = AddSpacesToCamelCase(Final);
	}

	return Final;
}

FText UGameplayTagFunctionLibrary::GetLastTextOfGameplayTag(const FGameplayTag& InGameplayTag, const bool bAddSpaceBetween)
{
	return FText::FromString(GetLastStringOfGameplayTag(InGameplayTag, bAddSpaceBetween));
}

FString UGameplayTagFunctionLibrary::MakeStringFromGameplayTag(const FGameplayTag& InGameplayTag,
	const FString& Separator, const int32 StartHierarchyLevel)
{
	TArray<FString> Strings = ParseGameplayTagToStrings(InGameplayTag);

	if (Strings.Num() <= 0) return FString();

	if (Strings.Num() == 1 && StartHierarchyLevel == 0) return Strings[0];

	FString Result;
	for (int32 I = StartHierarchyLevel; I < Strings.Num(); ++I)
	{
		if (!Strings.IsValidIndex(I)) break;

		Result += Strings[I];

		if (Strings.IsValidIndex(I + 1))
		{
			Result += Separator;
		}
	}

	return Result;
}

const FGameplayTag& UGameplayTagFunctionLibrary::GetFirstGameplayTagFromContainer(const FGameplayTagContainer& GameplayTagContainer)
{
	const TArray<FGameplayTag>& Tags = GameplayTagContainer.GetGameplayTagArray();
	if (Tags.IsValidIndex(0))
	{
		return Tags[0];
	}

	return FGameplayTag::EmptyTag;
}

FGameplayTag UGameplayTagFunctionLibrary::MakeTag(const FName& TagName)
{
	return FGameplayTag::RequestGameplayTag(TagName);
}

FGameplayTagContainer UGameplayTagFunctionLibrary::MakeTagContainer(const FName& Name)
{
	FGameplayTagContainer Container;
	Container.AddTag(MakeTag(Name));
	return Container;
}

FGameplayTagContainer UGameplayTagFunctionLibrary::MakeTagContainer(const TArray<FName>& Names)
{
	FGameplayTagContainer Container;
	for (const FName& Name : Names)
	{
		Container.AddTag(MakeTag(Name));
	}
	return Container;
}

FString UGameplayTagFunctionLibrary::AddSpacesToCamelCase(const FString& Input)
{
	FString Result;
	for (int32 I = 0; I < Input.Len(); ++I)
	{
		if (I > 0 && FChar::IsUpper(Input[I]) && Input[I - 1] != TEXT(' ')) 
		{
			// Add a space before uppercase letters only if the previous character is not a space
			Result += TEXT(" ");
		}
		Result += Input[I];
	}
	return Result;
}