// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/ExposeToBlueprintLibrary.h"

#include "NiagaraComponent.h"
#include "Engine/StaticMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ExposeToBlueprintLibrary)

bool UExposeToBlueprintLibrary::IsActorChildOf(const AActor* Actor, const TSubclassOf<AActor>& Class)
{
	if (!Actor || !Class) return false;

	return Actor->IsA(Class);
}

FVector UExposeToBlueprintLibrary::GetMeshSize(const UStaticMesh* Mesh)
{
	if (!Mesh) return FVector();

	return GetBoxSize(Mesh->GetBoundingBox());
}

FVector UExposeToBlueprintLibrary::GetBoxSize(const FBox& Box)
{
	return Box.GetSize();
}

FTransform UExposeToBlueprintLibrary::GetStaticMeshSocketWorldTransform(const UStaticMesh* StaticMesh, const FName& SocketName, const FTransform& CustomTransform)
{
	if (!StaticMesh) return CustomTransform;

	const UStaticMeshSocket* Socket = StaticMesh->FindSocket(SocketName);
	if (!Socket) return CustomTransform;

	return FTransform(Socket->RelativeRotation, Socket->RelativeLocation, Socket->RelativeScale) * CustomTransform;
}

void UExposeToBlueprintLibrary::DeactivateNiagaraImmediate(UNiagaraComponent* Niagara)
{
	if (Niagara) Niagara->DeactivateImmediate();
}

void UExposeToBlueprintLibrary::ToggleActorComponent(UActorComponent* ActorComp, const bool bActivated)
{
	if (!IsValid(ActorComp)) return;

	ActorComp->SetComponentTickEnabled(bActivated);
	bActivated ? ActorComp->Activate() : ActorComp->Deactivate();
}

void UExposeToBlueprintLibrary::SetPerBoneMotionBlur(USkeletalMeshComponent* SkeletalMeshComponent, const bool bEnabled)
{
	if (!SkeletalMeshComponent) return;

	SkeletalMeshComponent->bPerBoneMotionBlur = bEnabled;
}

FVector UExposeToBlueprintLibrary::GetPendingLaunchVel(const UCharacterMovementComponent* CharacterMoveComp)
{
	if (!CharacterMoveComp) return FVector();

	return CharacterMoveComp->PendingLaunchVelocity;
}

float UExposeToBlueprintLibrary::GetWorldGravity(const UObject* WorldContextObject)
{
	if (!WorldContextObject || !WorldContextObject->GetWorld()) return 0;

	return WorldContextObject->GetWorld()->GetGravityZ();
}