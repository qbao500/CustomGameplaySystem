// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

/**
 * Declare native tags here, using UE_DECLARE_GAMEPLAY_TAG_EXTERN.
 * Their definition is in CustomNativeGameplayTags.cpp file, using UE_DEFINE_GAMEPLAY_TAG or (preferably) UE_DEFINE_GAMEPLAY_TAG_COMMENT.
 * Must have CUSTOMGAMEPLAYSYSTEM_API before, so other modules can use it natively.
 */

namespace CustomTags
{
	// Major parent tags that are essential for GAS and related systems
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status);

	// Ability tags
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Behavior_SurvivesDeath);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Status);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Status_Locked);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Status_Eligible);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Status_Unlocked);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Status_Equipped);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Passive);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Active);

	// Attribute tags
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Meta);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Vital);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Vital_Health);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Vital_MaxHealth);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Vital_HealthRegen);

	// Input tags
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Ability);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Move);

	// Init States, mostly used for CustomPawnComponent_Base (which is a UPawnComponent)
	/** 1: Actor/component has initially spawned and can be extended */
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
	/** 2: All required data has been loaded/replicated and is ready for initialization */
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
	/** 3: The available data has been initialized for this actor/component, but it is not ready for full gameplay */
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
	/** 4: The actor/component is fully ready for active gameplay */
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);
	
	// Event tags
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Hit);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Character_Knockback);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Character_Death);	

	// Damage tags
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage);

	// Game-phase tags
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GamePhase);

	// Hitbox tags
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hitbox);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hitbox_Part);

	// Status tags
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_CC_Stun);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_CC_Root);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_CC_Disarm);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dying);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dead);

	// Save Game tags
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SaveGame);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SaveGame_Player);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SaveGame_Player_Ability);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SaveGame_Player_Attribute);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SaveGame_LevelExp);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SaveGame_WorldState);

	// Ability activation failed tags.
	// If you want to show why an ability failed to activate in Output Log and on the "showdebug AbilitySystem" HUD,
	// then add these in DefaultGame.ini, under [/Script/GameplayAbilities.AbilitySystemGlobals]
	// +ActivateFailIsDeadName=Ability.ActivateFail.IsDead
	// +ActivateFailCooldownName=Ability.ActivateFail.Cooldown
	// +ActivateFailCostName=Ability.ActivateFail.Cost
	// +ActivateFailTagsBlockedName=Ability.ActivateFail.TagsBlocked
	// +ActivateFailTagsMissingName=Ability.ActivateFail.TagsMissing
	// +ActivateFailNetworkingName=Ability.ActivateFail.Networking
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
	CUSTOMGAMEPLAYSYSTEM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Networking);

	CUSTOMGAMEPLAYSYSTEM_API	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);
}