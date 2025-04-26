// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SaveGame.h"
#include "CustomSaveGame.generated.h"

USTRUCT()
struct FActorSaveData
{
	GENERATED_BODY()
	
	/* Identifier for which Actor this belongs to */
	UPROPERTY()
	FName ActorName = NAME_None;

	/* For movable Actors, keep location, rotation, scale. */
	UPROPERTY()
	FTransform Transform = FTransform::Identity;

	/* Contains all 'SaveGame' marked UPROPERTY variables of the Actor */
	UPROPERTY()
	TArray<uint8> ByteData;

	bool operator==(const FActorSaveData& OtherData) const;
};

USTRUCT()
struct FWorldSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	FName WorldName = NAME_None;

	UPROPERTY()
	TMap<FName, FActorSaveData> ActorSaveDataMap;

	bool operator==(const FWorldSaveData& OtherData) const;
};

/**
 * TODO: Figure out what to put here, maybe SaveGameTag and/or SlotIndex?
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UCustomSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	TMap<FName, FWorldSaveData> WorldSaveDataMap;
};
