// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AsyncActions/TickableAsyncAction.h"
#include "AsyncLerpToAttachmentSocket.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMGAMEPLAYSYSTEM_API UAsyncLerpToAttachmentSocket : public UTickableAsyncAction
{
	GENERATED_BODY()

public:

	/**
	 * Lerp a scene component to a new parent component with optional socket.
	 * @param CompToAnimate The component we want to lerp smoothly to ParentComponent
	 * @param ParentComponent CompToAnimate will attach to this component
	 * @param Socket (Optional) Socket of ParentComponent
	 * @param Duration The time it takes for the lerp to complete
	 * @return The async node that will execute the lerp
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Flow Control")
	static UAsyncLerpToAttachmentSocket* AsyncLerpToAttachment(USceneComponent* CompToAnimate, USceneComponent* ParentComponent,
		const FName Socket = NAME_None, const float Duration = 0.2f);

	UPROPERTY(BlueprintAssignable)
	FFinished OnFinished;

	virtual void Activate() override;
	virtual void Tick(float DeltaTime) override;

private:

	float TotalDuration = 0.0f;
	float CurrentDuration = 0.0f;
	
	UPROPERTY()
	TObjectPtr<USceneComponent> ChildComp = nullptr;
	UPROPERTY()
	TObjectPtr<USceneComponent> ParentComp = nullptr;
	FName TargetSocket;
	
	/** The start transform in relative space of the target socket */
	FTransform StartTransformRelative;
};
