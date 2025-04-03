// Fill out your copyright notice in the Description page of Project Settings.

#include "Hitbox/HitboxComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "KismetTraceUtils.h"
#include "FunctionLibraries/PrintLogFunctionLibrary.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HitboxComponent)

void FHitboxInfo::ValidateInfo()
{
	LastSocketTraceLocation.Empty();
	
	for (TTuple<FName, float>& Pair : SocketNameScale)
	{
		const FName& SocketName = Pair.Key;
		float& Scale = Pair.Value;
		if (Scale <= 0.0f || FMath::IsNearlyZero(Scale))
		{
			Scale = 1.0f;
		}
		
		LastSocketTraceLocation.Emplace(SocketName);
	}
}

void FHitboxInfo::RefreshSocketTraceLocation()
{
	if (!CollidingComponent) return;

	for (TTuple<FName, FVector>& Pair : LastSocketTraceLocation)
	{
		Pair.Value = CollidingComponent->GetSocketLocation(Pair.Key);
	}
}

bool FHitboxInfo::HitboxTrace(const UObject* WorldContext, const float Radius, const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
	bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, TArray<FHitResult>& OutHits,
	EDrawDebugTrace::Type DrawDebugType, const FLinearColor& TraceColor, const FLinearColor& TraceHitColor, const float DrawTime)
{
	OutHits.Empty();
	bool bHit = false;

	for (TTuple<FName, FVector>& Pair : LastSocketTraceLocation)
	{
		const FName& Socket = Pair.Key;
		const FVector& Start = Pair.Value;
		const FVector& End = CollidingComponent->GetSocketLocation(Socket);

		TArray<FHitResult> Hits;
		if (UKismetSystemLibrary::SphereTraceMultiForObjects(WorldContext, Start, End, Radius * GetSocketScale(Socket), ObjectTypes, bTraceComplex,
			ActorsToIgnore, DrawDebugType, Hits, false, TraceColor, TraceHitColor, DrawTime))
		{
			bHit = true;
			OutHits.Append(Hits);
		}

		// Update location for next tick
		Pair.Value = End;
	}

	return bHit;
}

float FHitboxInfo::GetSocketScale(const FName& Socket) const
{
	if (const float* Scale = SocketNameScale.Find(Socket))
	{
		return *Scale;
	}

	return 1.0f;
}

void FHitboxInfo::Reset()
{
	CollidingComponent = nullptr;
	SocketNameScale.Empty();
	LastSocketTraceLocation.Empty();
}

#if WITH_EDITOR
void FHitboxInfo::PreviewDebug(const UObject* WorldContext, const float Radius, const float DrawTime)
{
	if (!CollidingComponent) return;

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return;

	for (TTuple<FName, FVector>& Pair : LastSocketTraceLocation)
	{
		const FName& Socket = Pair.Key;
		const FVector& Start = Pair.Value;
		const FVector& End = CollidingComponent->GetSocketLocation(Socket);

		TArray<FHitResult> Hits;
		const FLinearColor DrawColor = FLinearColor::Red;
		DrawDebugSphereTraceMulti(World, Start, End, Radius, EDrawDebugTrace::ForDuration, false, Hits, DrawColor, DrawColor, DrawTime);

		// Update location for next tick
		Pair.Value = End;
	}
}
#endif

UHitboxComponent::UHitboxComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	SetIsReplicatedByDefault(true);

	ObjectTypesToCollideWith.Emplace(ECC_Pawn);
}

void UHitboxComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UHitboxComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;

	PrimaryComponentTick.TickInterval = TraceCheckInterval;

	IgnoringActors.Emplace(GetOwner());
	
	for (TTuple<FGameplayTag, FHitboxInfo>& Pair : HitboxPartInfo)
	{
		FHitboxInfo& Info = Pair.Value;

		if (!Info.bAutoUseCharacterMesh) continue;

		// Auto assign CollidingComponent with Character Mesh
		if (const ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner()))
		{
			Info.CollidingComponent = CharacterOwner->GetMesh();

			// Just for safe
			Info.ValidateInfo();
		}
	}
}

void UHitboxComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!HasAuthority()) return;

	if (!CurrentHitboxPart.IsValid()) return;

	FHitboxInfo* Info = HitboxPartInfo.Find(CurrentHitboxPart);
	if (!Info) return;
	if (!Info->CollidingComponent) return;

	TArray<FHitResult> OutHits;
	if (Info->HitboxTrace(GetOwner(), TraceRadius, ObjectTypesToCollideWith, bTraceComplex, IgnoringActors,
		OutHits, DrawDebugType, TraceColor, TraceHitColor, DrawTime))
	{
		for (const FHitResult& Hit : OutHits)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor) continue;

			if (AlreadyHitActors.Contains(HitActor)) continue;
				
			if (IgnoreClasses.Contains(HitActor->GetClass()))
			{
				// Add to "ignore" this actor when it hits again
				AlreadyHitActors.Emplace(HitActor);
				continue;
			}

			// Hit once
			// TODO Add feature to support getting hit multiple time per collision window
			AlreadyHitActors.Emplace(HitActor);
			
			OnHit.Broadcast(Hit, Info->CollidingComponent, CurrentHitboxPart);

			// Event sending
			if (!EventToSendWhenHit.IsValid()) continue;
			FGameplayEventData Payload;
			Payload.EventTag = EventToSendWhenHit;
			Payload.Target = HitActor;
			Payload.OptionalObject = this;
			Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(Hit);
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), EventToSendWhenHit, Payload);
		}
	}
}

#if WITH_EDITOR
void UHitboxComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	for (TTuple<FGameplayTag, FHitboxInfo>& HitBoxPair : HitboxPartInfo)
	{
		HitBoxPair.Value.ValidateInfo();
	}
}
#endif

void UHitboxComponent::SetupComponentForHitboxPart(const FGameplayTag& HitboxPart, UPrimitiveComponent* Component)
{
	if (!Component) return;

	if (!HasAuthority()) return;
	
	if (FHitboxInfo* Info = HitboxPartInfo.Find(HitboxPart))
	{
		// Always override
		Info->bAutoUseCharacterMesh = false;
		
		Info->CollidingComponent = Component;

		// Just for safe
		Info->ValidateInfo();
	}
}

void UHitboxComponent::ActivateCollision(const FGameplayTag& HitboxPart)
{
	if (!HasAuthority()) return;

	for (TTuple<FGameplayTag, FHitboxInfo>& HitBoxPair : HitboxPartInfo)
	{
		HitBoxPair.Value.RefreshSocketTraceLocation();
	}

	AlreadyHitActors.Empty();
	SetComponentTickEnabled(true);

	OnCollisionActivated.Broadcast(HitboxPart);
	CurrentHitboxPart = HitboxPart;
}

void UHitboxComponent::DeactivateCollision()
{
	if (!HasAuthority()) return;

	SetComponentTickEnabled(false);
	AlreadyHitActors.Empty();

	OnCollisionDeactivated.Broadcast(CurrentHitboxPart);
	CurrentHitboxPart = FGameplayTag::EmptyTag;
}

FHitboxInfo UHitboxComponent::GetHitboxInfo(const FGameplayTag& HitboxPart) const
{
	return HitboxPartInfo.FindRef(HitboxPart);
}

float UHitboxComponent::GetTraceRadius() const
{
	return TraceRadius;
}

bool UHitboxComponent::HasAuthority() const
{
	return GetOwner()->HasAuthority();
}
