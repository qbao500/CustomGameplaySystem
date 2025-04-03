// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HitboxComponent.generated.h"


USTRUCT(BlueprintType)
struct FHitboxInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TMap<FName, float> SocketNameScale;

	// Auto use Character Mesh for socket trace if true. Otherwise, you need to call SetupComponentForHitboxPart().
	UPROPERTY(EditAnywhere)
	bool bAutoUseCharacterMesh = true;

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> CollidingComponent;

	void ValidateInfo();

	TMap<FName, FVector> LastSocketTraceLocation;
	void RefreshSocketTraceLocation();
	
	bool HitboxTrace(const UObject* WorldContext, const float Radius, const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
		bool bTraceComplex,	const TArray<AActor*>& ActorsToIgnore, TArray<FHitResult>& OutHits,
		EDrawDebugTrace::Type DrawDebugType, const FLinearColor& TraceColor = FLinearColor::Red,
		const FLinearColor& TraceHitColor = FLinearColor::Green, const float DrawTime = 5.0f);

	float GetSocketScale(const FName& Socket) const;
	
	void Reset();

#if WITH_EDITOR
	void PreviewDebug(const UObject* WorldContext, const float Radius, const float DrawTime = 5.0f);
#endif
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollisionActivation, const FGameplayTag&, CollisionType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCollisionHit, const FHitResult&, HitResult, UPrimitiveComponent*, CollidingComponent, const FGameplayTag&, HitboxPart);

/**
 * A hitbox component that use sphere trace for collision detection.
 * If use with GAS, it also sends events when it hits something (mostly to an Ability that's running and listening to that event).
 * Coded so it only runs on Server and Standalone game.
 */
UCLASS(ClassGroup = (Hitbox), meta = (BlueprintSpawnableComponent), BlueprintType)
class CUSTOMGAMEPLAYSYSTEM_API UHitboxComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UHitboxComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// Setup component for HitboxPartInfo
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "HitboxPart", Categories = "Hitbox.Part"))
	void SetupComponentForHitboxPart(const FGameplayTag& HitboxPart, UPrimitiveComponent* Component);
	
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "HitboxPart", Categories = "Hitbox.Part"))
	void ActivateCollision(const FGameplayTag& HitboxPart);
	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	UFUNCTION(BlueprintPure, meta = (AutoCreateRefTerm = "HitboxPart", Categories = "Hitbox.Part"))
	FHitboxInfo GetHitboxInfo(const FGameplayTag& HitboxPart) const;
	UFUNCTION(BlueprintPure)
	float GetTraceRadius() const;

	// As it's coded right now, these events are only called on Server. Reconsider if it should call on Client also.
	UPROPERTY(BlueprintAssignable)
	FCollisionActivation OnCollisionActivated;
	UPROPERTY(BlueprintAssignable)
	FCollisionActivation OnCollisionDeactivated;
	UPROPERTY(BlueprintAssignable)
	FCollisionHit OnHit;

protected:

	// Define Sockets for Collision type. Must call SetupComponentForHitboxPart in owner once to assign Component for collision
	UPROPERTY(EditDefaultsOnly, meta = (ForceInlineRow, Categories = "Hitbox.Part"))
	TMap<FGameplayTag, FHitboxInfo> HitboxPartInfo;

	// Whether to use complex trace or not
	UPROPERTY(EditDefaultsOnly)
	bool bTraceComplex = false;

	// Radius of sphere trace checking collision
	UPROPERTY(EditDefaultsOnly)
	float TraceRadius = 5.0f;

	// How often there is trace check while collision is activated
	// If <= 0, then every tick
	UPROPERTY(EditDefaultsOnly)
	float TraceCheckInterval = 0.033f;

	// Actor classes that will be ignored
	UPROPERTY(EditDefaultsOnly)
	TSet<TSoftClassPtr<AActor>> IgnoreClasses;

	// Types of objects to collide with
	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesToCollideWith;

	// Only needed if use with GAS, specially DamageGameplayAbility
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "Event"))
	FGameplayTag EventToSendWhenHit = FGameplayTag::EmptyTag;

	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType = EDrawDebugTrace::None;
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	FLinearColor TraceColor = FLinearColor::Red;
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	FLinearColor TraceHitColor = FLinearColor::Green;
	UPROPERTY(EditDefaultsOnly, Category = "Debug", meta = (EditCondition = "DrawDebugType == EDrawDebugTrace::ForDuration", EditConditionHides))
	float DrawTime = 5.0f;

	FGameplayTag CurrentHitboxPart = FGameplayTag::EmptyTag;
	
	UPROPERTY()
	TSet<AActor*> AlreadyHitActors;
	UPROPERTY()
	TArray<AActor*> IgnoringActors;

	bool HasAuthority() const;

	friend class ANS_HitboxWindow;
};