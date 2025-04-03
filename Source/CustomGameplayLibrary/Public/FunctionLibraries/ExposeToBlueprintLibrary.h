// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ExposeToBlueprintLibrary.generated.h"

class UCharacterMovementComponent;
class UNiagaraComponent;

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYLIBRARY_API UExposeToBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure)
	static bool IsActorChildOf(const AActor* Actor, const TSubclassOf<AActor>& Class);
	UFUNCTION(BlueprintPure)
	static FVector GetMeshSize(const UStaticMesh* Mesh);
	UFUNCTION(BlueprintPure)
	static FVector GetBoxSize(const FBox& Box);
	UFUNCTION(BlueprintPure)
	static FTransform GetStaticMeshSocketWorldTransform(const UStaticMesh* StaticMesh, const FName& SocketName, const FTransform& CustomTransform);
	UFUNCTION(BlueprintCallable)
	static void DeactivateNiagaraImmediate(UNiagaraComponent* Niagara);
	UFUNCTION(BlueprintCallable)
	static void ToggleActorComponent(UActorComponent* ActorComp, const bool bActivated);
	UFUNCTION(BlueprintCallable)
	static void SetPerBoneMotionBlur(USkeletalMeshComponent* SkeletalMeshComponent, const bool bEnabled);
	UFUNCTION(BlueprintPure)
	static FVector GetPendingLaunchVel(const UCharacterMovementComponent* CharacterMoveComp);
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static float GetWorldGravity(const UObject* WorldContextObject);
};
