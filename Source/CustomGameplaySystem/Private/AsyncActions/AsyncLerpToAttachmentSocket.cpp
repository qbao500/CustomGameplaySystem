// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncActions/AsyncLerpToAttachmentSocket.h"

#include "Kismet/KismetMathLibrary.h"

UAsyncLerpToAttachmentSocket* UAsyncLerpToAttachmentSocket::AsyncLerpToAttachment(USceneComponent* CompToAnimate,
	USceneComponent* ParentComponent, const FName Socket, const float Duration)
{
	if (!CompToAnimate || !ParentComponent || Duration <= 0.0f) return nullptr;
	
	UAsyncLerpToAttachmentSocket* Action = NewObject<UAsyncLerpToAttachmentSocket>();
	Action->TotalDuration = Duration;
	Action->ChildComp = CompToAnimate;
	Action->ParentComp = ParentComponent;
	Action->TargetSocket = Socket;

	Action->RegisterWithGameInstance(CompToAnimate);
	
	return Action;
}

void UAsyncLerpToAttachmentSocket::Activate()
{
	Super::Activate();
	
	// Immediately attach to the new target socket; we use "KeepWorldTransform" so the component doesn't snap visibility
	ChildComp->AttachToComponent(ParentComp, FAttachmentTransformRules::KeepWorldTransform, TargetSocket);
  
	StartTransformRelative = ChildComp->GetRelativeTransform();
}

void UAsyncLerpToAttachmentSocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float Alpha = FMath::Clamp<float>(CurrentDuration / TotalDuration, 0.0f, 1.0f);
	
	// The target transform is default initialized as it represents the socket transform
	const FTransform NewTransformInRelative = UKismetMathLibrary::TLerp(StartTransformRelative, FTransform(), Alpha, ELerpInterpolationMode::DualQuatInterp);
	ChildComp->SetRelativeLocationAndRotation(NewTransformInRelative.GetLocation(), NewTransformInRelative.GetRotation());
	
	CurrentDuration += DeltaTime;
	
	if (CurrentDuration >= TotalDuration)
	{
		OnFinished.Broadcast();
		Cancel();
	}
}
