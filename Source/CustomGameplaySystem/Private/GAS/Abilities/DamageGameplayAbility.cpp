// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/DamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "CustomGameplayTags.h"
#include "Abilities/Async/AbilityAsync_WaitGameplayEvent.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "GAS/CustomAbilitySystemLibrary.h"
#include "GAS/Components/CustomAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DamageGameplayAbility)

float FAbilityDamageInfo::GetCurrentDamage() const
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
}

float FAbilityDamageInfo::GetDamageAtLevel(const int32 Level) const
{
	return Damage.GetValueAtLevel(Level);
}

int32 FAbilityDamageInfo::GetAbilityLevel() const
{
	return DamageContextHandle.GetAbilityLevel();
}

UAbilitySystemComponent* FAbilityDamageInfo::GetAbilitySystemComponent() const
{
	return DamageContextHandle.GetOriginalInstigatorAbilitySystemComponent();
}

const UGameplayAbility* FAbilityDamageInfo::GetAbility() const
{
	return DamageContextHandle.GetAbility();
}

FText UDamageGameplayAbility::FillAbilityDescriptionWithFormat_Implementation(const FText& Description, int32 Level)
{
	Super::FillAbilityDescriptionWithFormat_Implementation(Description, Level);
	
	FFormatNamedArguments Args;
	Args.Add(DamageKeyToFormat, GetTotalDamageAtLevel(Level));
	
	return FText::Format(Description, Args);
}

void UDamageGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	// Default DamageContext
	const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo_Checked();
	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.SetAbility(this);
	Context.AddSourceObject(this);
	DamageInfo.DamageContextHandle = Context;
	
	if (bAutoListenToHitEvent)
	{
		WaitHitEventAction = UAbilityAsync_WaitGameplayEvent::WaitGameplayEventToActor(GetOwningActorFromActorInfo(), CustomTags::Event_Hit);
		if (!WaitHitEventAction) return;

		// Bind to the event
		WaitHitEventAction->EventReceived.AddDynamic(this, &ThisClass::OnHitTargetEvent);

		// For some reason, Activate() is not public in UAbilityAsync_WaitGameplayEvent, so we need to cast it back to UBlueprintAsyncActionBase
		// This is guaranteed to be valid, since UAbilityAsync_WaitGameplayEvent is a subclass of UBlueprintAsyncActionBase
		UBlueprintAsyncActionBase* BaseAction = Cast<UBlueprintAsyncActionBase>(WaitHitEventAction);
		BaseAction->Activate();
	}
}

void UDamageGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);

	if (WaitHitEventAction)
	{
		WaitHitEventAction->EventReceived.RemoveAll(this);
		WaitHitEventAction->Cancel();
		WaitHitEventAction = nullptr;
	}
}

void UDamageGameplayAbility::OnHitTargetEvent(FGameplayEventData Payload)
{
	AutoApplyEffectsOnHit(Payload);
	
	// BP
	K2_OnHitTargetEvent(Payload);
}

float UDamageGameplayAbility::GetTotalDamage() const
{
	return DamageInfo.Damage.GetValueAtLevel(GetAbilityLevel());
}

float UDamageGameplayAbility::GetTotalDamageAtLevel(const int32 Level) const
{
	return DamageInfo.Damage.GetValueAtLevel(Level);
}

void UDamageGameplayAbility::AssignDamageToEffectSpec(const FGameplayEffectSpecHandle& SpecHandle) const
{
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (!Spec) return;
	
	const float ScaledDamage = DamageInfo.Damage.GetValueAtLevel(GetAbilityLevel());
	Spec->SetSetByCallerMagnitude(DamageInfo.DamageTag, ScaledDamage);
}

void UDamageGameplayAbility::AssignCustomDamageToEffectSpec(const FGameplayEffectSpecHandle& SpecHandle, const float CustomDamage)
{
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (!Spec) return;

	Spec->SetSetByCallerMagnitude(DamageInfo.DamageTag, CustomDamage);
}

void UDamageGameplayAbility::ApplyDamage(AActor* TargetActor, const FHitResult& HitResult)
{
	if (!HasAuthority(&CurrentActivationInfo)) return;

	AFL::AbilityApplyDamage(this, DamageInfo, TargetActor, HitResult);
}

bool UDamageGameplayAbility::ApplyRadialDamage(const FVector& Origin, const TArray<AActor*>& IgnoreActors)
{
	if (!HasAuthority(&CurrentActivationInfo)) return false;
	
	return AFL::AbilityApplyRadialDamage(this, DamageInfo, Origin, IgnoreActors);
}

void UDamageGameplayAbility::AutoApplyEffectsOnHit(const FGameplayEventData& Payload)
{
	if (EffectsToAutoApplyToHitTarget.Num() <= 0) return;

	// Only apply if damage dealt is bigger than 0
	if (Payload.EventMagnitude <= 0.0f) return;

	const AActor* Target = Payload.Target;
	if (!Target) return;

	UCustomAbilitySystemComponent* TargetASC = UCustomAbilitySystemComponent::GetCustomAbilityComponent(Target);
	if (!TargetASC) return;
	
	// Only apply effects if hit event is cause by this ability
	if (!DoesEffectContextHaveThisAbility(Payload.ContextHandle)) return;

	UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo_Checked();
	
	for (const TSubclassOf<UGameplayEffect>& Effect : EffectsToAutoApplyToHitTarget)
	{
		if (!Effect) continue;

		FGameplayEffectSpecHandle EffectSpec = OwnerASC->MakeOutgoingSpec(Effect, GetAbilityLevel(), Payload.ContextHandle);
		OwnerASC->ApplyGameplayEffectSpecToTarget(*EffectSpec.Data, TargetASC);
	}
}