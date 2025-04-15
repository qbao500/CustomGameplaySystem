// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomGameplayTags.h"

/**
 * Definition of custom native tags is here.
 */

namespace CustomTags
{
	// Major parent tags that are essential for GAS and related systems
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability, "Ability", "Tags owned by Ability System Component through Gameplay Ability");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute, "Attribute", "Tags for Attributes, if needed");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input, "Input", "Input Tag, if needed");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message, "Message", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event, "Event", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status, "Status", "");

	// Ability tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Behavior_SurvivesDeath, "Ability.Behavior.SurvivesDeath", "An ability with this type tag should not be canceled due to death.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Status, "Ability.Status", "If needed, this can be used for unlocking and equipping abilities");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Status_Locked, "Ability.Status.Locked", "Ability is locked and cannot be unlocked until meeting requirements");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Status_Eligible, "Ability.Status.Eligible", "Ability is eligible to be unlocked after meeting requirements");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Status_Unlocked, "Ability.Status.Unlocked", "Ability is unlocked but not equipped");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Status_Equipped, "Ability.Status.Equipped", "Ability is unlocked and equipped");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Type_Passive, "Ability.Type.Passive", "Passive ability type");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Type_Active, "Ability.Type.Active", "Active ability type, can be triggered by input");

	// Input tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Move, "Input.Move", "Move input tag");

	// Attribute tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Meta, "Attribute.Meta", "Tags for Meta Attributes, if needed");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Vital, "Attribute.Vital", "Tags for Vital Attributes");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Vital_Health, "Attribute.Vital.Health", "Health attribute");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Vital_MaxHealth, "Attribute.Vital.MaxHealth", "Max Health attribute");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Vital_HealthRegen, "Attribute.Vital.HealthRegen", "Health Regeneration attribute");

	// Init States, mostly used for CustomPawnComponent_Base (which is a UPawnComponent)
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_Spawned, "InitState.Spawned", "1: Actor/component has initially spawned and can be extended");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_DataAvailable, "InitState.DataAvailable", "2: All required data has been loaded/replicated and is ready for initialization");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_DataInitialized, "InitState.DataInitialized", "3: The available data has been initialized for this actor/component, but it is not ready for full gameplay");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_GameplayReady, "InitState.GameplayReady", "4: The actor/component is fully ready for active gameplay");

	// Event tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Hit, "Event.Hit", "Event when Ability hit target");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Character_Knockback, "Event.Character.Knockback", "Knockback event for Characters");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Character_Death, "Event.Character.Death", "Death event for Characters");

	// Damage tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage, "Damage", "Parent tag of damage types. You should also use this or child tags for SetByCaller");

	// Game-phase tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GamePhase, "GamePhase", "Parent tag for game phases");

	// Hitbox tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Hitbox, "Hitbox", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Hitbox_Part, "Hitbox.Part", "Part of the body or weapon for hitbox to check (left arm, right foot, etc.). Add new parts per project needs");

	// Status CC tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_CC_Stun, "Status.CC.Stun", "Can't move, can't attack");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_CC_Root, "Status.CC.Root", "Can't move, still can attack");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_CC_Disarm, "Status.CC.Disarm", "Can move, but can't attack");

	// Status Death tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death_Dying, "Status.Death.Dying", "Target has begun the death process.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death_Dead, "Status.Death.Dead", "Target has finished the death process.");

	// Save Game tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SaveGame, "SaveGame", "Use child tag as ID for which type of save game is being loaded/saved.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SaveGame_Player, "SaveGame.Player", "Save game for player data. Can have children to separate data types.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SaveGame_Player_Ability, "SaveGame.Player.Ability", "Save ability status for Player. Default usage is in AbilityStatusComponent.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SaveGame_Player_Attribute, "SaveGame.Player.Attribute", "Save attribute status for Player. Not using anywhere in CustomGameplaySystem plugin.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SaveGame_LevelExp, "SaveGame.LevelExp", "Used in LevelExpComponent. Make children tags for different character that need to use this component.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SaveGame_WorldState, "SaveGame.WorldState", "ID for saving world state. Used by SaveLoadSubsystem->SaveWorldState()");

	// Ability activation failed tags.
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_IsDead, "Ability.ActivateFail.IsDead", "Ability failed to activate because its owner is dead.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Cooldown, "Ability.ActivateFail.Cooldown", "Ability failed to activate because it is on cool down.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Cost, "Ability.ActivateFail.Cost", "Ability failed to activate because it did not pass the cost checks.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_TagsBlocked, "Ability.ActivateFail.TagsBlocked", "Ability failed to activate because tags are blocking it.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_TagsMissing, "Ability.ActivateFail.TagsMissing", "Ability failed to activate because tags are missing.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Networking, "Ability.ActivateFail.Networking", "Ability failed to activate because it did not pass the network checks.");

	FGameplayTag FindTagByString(const FString& TagString, const bool bMatchPartialString)
	{
		const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
		FGameplayTag Tag = Manager.RequestGameplayTag(FName(*TagString), false);

		if (!Tag.IsValid() && bMatchPartialString)
		{
			FGameplayTagContainer AllTags;
			Manager.RequestAllGameplayTags(AllTags, true);

			for (const FGameplayTag& TestTag : AllTags)
			{
				if (TestTag.ToString().Contains(TagString))
				{
					UE_LOG(LogTemp, Display, TEXT("Could not find exact match for tag [%s] but found partial match on tag [%s]."), *TagString, *TestTag.ToString());
					Tag = TestTag;
					break;
				}
			}
		}

		return Tag;
	}
}