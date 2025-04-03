// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveLoadSystem/CustomSaveGame.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomSaveGame)

bool FActorSaveData::operator==(const FActorSaveData& OtherData) const
{
	return ActorName == OtherData.ActorName;
}

bool FWorldSaveData::operator==(const FWorldSaveData& OtherData) const
{
	return WorldName == OtherData.WorldName;
}
