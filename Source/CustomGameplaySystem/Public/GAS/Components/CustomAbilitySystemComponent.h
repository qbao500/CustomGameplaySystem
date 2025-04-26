// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"
#include "CustomAbilitySystemComponent.generated.h"

class UCustomAbilityTagRelationship;
class UCustomGameplayAbility;
class UCustomAbilityInfoDataAsset;

DECLARE_DYNAMIC_DELEGATE_OneParam(FAbilityGiven, const FGameplayAbilitySpec&, AbilitySpec);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityGivenBroadcast, const FGameplayAbilitySpec&, AbilitySpec);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityRemoved, const FGameplayAbilitySpec&, AbilitySpec);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAbilityUpgraded, const FGameplayTag&, AbilityTag, const int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAttributeUpgraded, const FGameplayTag&, AttibuteTag, const int32, UpgradedAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAbilityInputTagChanged, const FGameplayTag&, AbilityTag, const FGameplayAbilitySpec&, AbilitySpec, const FGameplayTag&, NewInputTag);

UENUM(BlueprintType)
enum class EAbilityUpgradeMethod : uint8
{
	// Instantly increase the level of the ability, even while it's active
	Default,
	// If the ability is active, then cancel it first before upgrading
	CancelAbility,
};

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UCustomAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	//~ Begin UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End UActorComponent interface

	//~ Begin UAbilitySystemComponent interface
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility,
		bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
	//~ End UAbilitySystemComponent interface
	
	UFUNCTION(BlueprintPure, Category = "Custom|AbilitySystem")
	static UCustomAbilitySystemComponent* GetCustomAbilityComponent(const AActor* Actor);

	/** Gets the ability target data associated with the given ability handle and activation info */
	void GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, const FGameplayAbilityActivationInfo& ActivationInfo,
		FGameplayAbilityTargetDataHandle& OutTargetDataHandle) const;

	/** Looks at ability tags and gathers additional required and blocking tags */
	void GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const;

	typedef TFunctionRef<bool(const UCustomGameplayAbility* CustomAbility, FGameplayAbilitySpecHandle Handle)> TShouldCancelAbilityFunc;
	void CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility);

	UFUNCTION(BlueprintCallable, Category = "Custom|AbilitySystem", meta = (AutoCreateRefTerm = "AbilityTag", Categories = "Ability"))
	void UpgradeAbilityLevel(const FGameplayTag& AbilityTag, EAbilityUpgradeMethod UpgradeMethod = EAbilityUpgradeMethod::Default,
		const int32 UpgradeAmount = 1);
	void UpgradeAbilityLevel(FGameplayAbilitySpec& AbilitySpec, EAbilityUpgradeMethod UpgradeMethod = EAbilityUpgradeMethod::Default,
		const int32 UpgradeAmount = 1);
	void UpgradeAbilityLevel(FGameplayAbilitySpec* AbilitySpec, EAbilityUpgradeMethod UpgradeMethod = EAbilityUpgradeMethod::Default,
		const int32 UpgradeAmount = 1);
	UPROPERTY(BlueprintAssignable)
	FAbilityUpgraded OnAbilityLevelUpgraded;

	// Require to subclass UCustomAbilitySystemGlobals and override InitAttributeTags()
	UFUNCTION(BlueprintCallable, Category = "Custom|AbilitySystem", meta = (AutoCreateRefTerm = "AttributeTag"))
	void UpgradeAttribute(const FGameplayTag& AttributeTag, const float UpgradeAmount = 1);
	void SetAttributeValue(const FGameplayTag& AttributeTag, const float NewValue);
	void SetAttributeValues(const TMap<FGameplayTag, float>& AttributeValueMap);
	UPROPERTY(BlueprintAssignable)
	FAttributeUpgraded OnAttributeUpgraded;

	// This load UCustomAbilityInfoDataAsset from Asset Manager, also assign StartingInputTag (from data) if valid.
	// If OptionalInputTag is valid, then use it instead of StartingInputTag.
	// If the ability is already given, then switch input tag to OptionalInputTag if valid.
	// If either InputTag is valid, then remove the abilities with same input tag.
	UFUNCTION(BlueprintCallable, Category = "Custom|AbilitySystem", meta = (AutoCreateRefTerm = "AbilityTag, OptionalInputTag"))
	void GiveAbilityByTag(const FGameplayTag& AbilityTag, const int32 Level = 1, const FGameplayTag& OptionalInputTag = FGameplayTag());
	UFUNCTION(BlueprintCallable, Category = "Custom|AbilitySystem")
	void ListenToAbilityGivenEvent(const FAbilityGiven& Delegate);

	UFUNCTION(BlueprintCallable, Category = "Custom|AbilitySystem", meta = (AutoCreateRefTerm = "AbilityTag", Categories = "Ability"))
	void RemoveAbilityByTag(const FGameplayTag& AbilityTag);
	UPROPERTY(BlueprintAssignable, Category = "Custom|AbilitySystem")
	FAbilityRemoved OnAbilityRemoved;

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAllInputHandles();

	void ClearAbilityInputTag(FGameplayAbilitySpec* AbilitySpec, const bool bMarkSpecDirty = true);
	void RemoveAbilitiesWithInputTag(const FGameplayTag& InputTag);

	// Event when the input tag of the given ability is changed.
	// Input tag is added in AbilitySpec.DynamicAbilityTags of the ability.
	UPROPERTY(BlueprintAssignable, Category = "Custom|AbilitySystem|Input")
	FAbilityInputTagChanged OnAbilityInputTagChanged;

	/** Sets the current tag relationship mapping, if null it will clear it out */
	void SetTagRelationshipMapping(UCustomAbilityTagRelationship* NewMapping);

protected:

	// If set, this table is used to look up tag relationships for activate and cancel
	UPROPERTY()
	TObjectPtr<UCustomAbilityTagRelationship> TagRelationshipMapping;

	virtual void TryActivateAbilitiesOnSpawn();

	virtual void Internal_UpgradeAbilityLevel(FGameplayAbilitySpec& AbilitySpec, EAbilityUpgradeMethod UpgradeMethod, const int32 UpgradeAmount);
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_BroadcastOnAbilityUpgraded(const FGameplayTag& AbilityTag, const int UpgradedAmount);

	UFUNCTION(Server, Reliable)
	virtual void Server_UpgradeAttribute(const FGameplayTag& AttributeTag, const float UpgradeAmount);
	virtual bool CanUpgradeAttribute(const FGameplayTag& AttributeTag) const;

	UFUNCTION(Server, Reliable)
	virtual void Server_SetAttributeValue(const FGameplayAttribute& Attribute, const float NewValue);
	UFUNCTION(Server, Reliable)
	virtual void Server_SetAttributeValues(const TArray<FGameplayTag>& AttributeTags, const TArray<float>& AttributeValues);

	UFUNCTION(Client, Reliable)
	void Client_BroadcastAbilityInputChanged(const FGameplayTag& AbilityTag, const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& NewInputTag);
	void SetInputTagForGivenAbility(FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& InputTag);
	void SetInputTagForGivenAbility(const FGameplayTag& AbilityTag, FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& InputTag);

	UFUNCTION(BlueprintPure, meta = (AutoCreateRefTerm = "AttributeTag", Categories = "Attribute"))
	static const FGameplayAttribute& GetAttributeFromTag(const FGameplayTag& AttributeTag);

	UAssetManager& GetAssetManager() const;

private:

	// Handles to abilities that had their input pressed this frame.
	TSet<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	// Handles to abilities that had their input released this frame.
	TSet<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	// Handles to abilities that have their input held.
	TSet<FGameplayAbilitySpecHandle> InputHeldSpecHandles;
	
	FAbilityGivenBroadcast OnAbilityGiven;
};