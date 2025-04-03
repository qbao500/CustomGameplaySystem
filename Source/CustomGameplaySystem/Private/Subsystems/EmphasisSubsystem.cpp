// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/EmphasisSubsystem.h"

#include "Kismet/GameplayStatics.h"


void UEmphasisSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	CameraManager = UGameplayStatics::GetPlayerCameraManager(&InWorld, 0);
}

void UEmphasisSubsystem::RunConstantSlomo(const float Rate, const float Duration)
{
	CancelSlomo();
	
	SlomoStartTime = GetWorld()->GetRealTimeSeconds();
	RealSlomoMechanic(Rate);
	GetWorld()->GetTimerManager().SetTimer(SlomoHandle, this, &UEmphasisSubsystem::StopSlomo, Duration * Rate, false);
}

void UEmphasisSubsystem::RunDecayingSlomo(float Rate, float Duration, const float FirstDelay)
{
	DurationTracker = 0;
	if (FirstDelay > 0.0f)
	{
		// Set timer for the first delay
		FTimerHandle FirstDelayHandle;
		const FTimerDelegate TimerDel = FTimerDelegate::CreateUObject(this, &UEmphasisSubsystem::StartSlomoDecay, Duration, Rate);
		GetWorld()->GetTimerManager().SetTimer(FirstDelayHandle, TimerDel, FirstDelay, false);	
	}
	else
	{
		StartSlomoDecay(Duration, Rate);
	}	
}

void UEmphasisSubsystem::CancelSlomo()
{
	GetWorld()->GetTimerManager().ClearTimer(SlomoHandle);
	StopSlomo();
}

bool UEmphasisSubsystem::IsInSlomo() const
{
	return GetTimerManager().IsTimerActive(SlomoHandle);
}

void UEmphasisSubsystem::OverrideSlomoGlobalPitch(const float Pitch)
{
	OverridenGlobalPitch = Pitch;
}

float UEmphasisSubsystem::GetOverridenGlobalPitch() const
{
	return (OverridenGlobalPitch > 0.0f) ? OverridenGlobalPitch : 0.75f;
}

void UEmphasisSubsystem::HitStop(AActor* Caller, AActor* Target, const float Duration, const float FirstDelay)
{
	if (!Caller || !Target || Duration <= 0.0f) return;
	
	// Don't call this if in slow motion
	if (IsInSlomo()) return;

	// Don't allow to hit-stop again
	if (HitStoppingActors.Contains(Caller) || HitStoppingActors.Contains(Target)) return;

	// Now cache
	HitStoppingActors.Emplace(Caller);
	HitStoppingActors.Emplace(Target);

	// Hit stop
	if (FirstDelay > 0.0f)
	{
		FTimerHandle FirstDelayHandle;
		const FTimerDelegate TimerDel = FTimerDelegate::CreateUObject(this, &UEmphasisSubsystem::BeginHitStop, Caller, Target, Duration);
		GetWorld()->GetTimerManager().SetTimer(FirstDelayHandle, TimerDel, FirstDelay, false);	
	}
	else
	{
		BeginHitStop(Caller, Target, Duration);
	}
}

void UEmphasisSubsystem::CancelAllHitStop()
{
	for (AActor* Actor : HitStoppingActors)
	{
		if (!Actor) continue;

		Actor->CustomTimeDilation = 1.0f;
	}

	HitStoppingActors.Empty();
}

// TODO Make settings (for real players) for camera shake scale 
UCameraShakeBase* UEmphasisSubsystem::StartCameraShake(const TSubclassOf<UCameraShakeBase> ShakeClass, const float Scale,
	const ECameraShakePlaySpace PlaySpace, const FRotator UserPlaySpaceRot)
{
	if (!CameraManager) return nullptr;

	return CameraManager->StartCameraShake(ShakeClass, Scale, PlaySpace, UserPlaySpaceRot);
}

UEmphasisSubsystem* UEmphasisSubsystem::GetEmphasisSubsystem(const AActor* ContextActor)
{
	if (!ContextActor && !ContextActor->GetWorld()) return nullptr;

	return ContextActor->GetWorld()->GetSubsystem<UEmphasisSubsystem>();
}

void UEmphasisSubsystem::StopSlomo() const
{
	RealSlomoMechanic(1.0f);
}

void UEmphasisSubsystem::StartSlomoDecay(float InDuration, const float InRate)
{
	CancelSlomo();
	
	SlomoStartTime = SlomoTickTime = GetWorld()->GetRealTimeSeconds();
	CurrentSlomoRate = InRate;
	RealSlomoMechanic(InRate);
	const FTimerDelegate TimerDel = FTimerDelegate::CreateUObject(this, &UEmphasisSubsystem::ReturningToNormalTime, InDuration);
	GetWorld()->GetTimerManager().SetTimer(SlomoHandle, TimerDel, SlomoTickRate * InRate, false);
}

void UEmphasisSubsystem::ReturningToNormalTime(float Duration)
{
	const float DurationSinceLastTick = GetWorld()->GetRealTimeSeconds() - SlomoTickTime;
	SlomoTickTime = GetWorld()->GetRealTimeSeconds();
	
	DurationTracker += DurationSinceLastTick;
	//UE_LOG(LogTemp, Warning, TEXT("DurationTracker: %f"), DurationTracker);
	//UE_LOG(LogTemp, Warning, TEXT("Duration: %f"), Duration);
	//UE_LOG(LogTemp, Warning, TEXT("Alpha: %f"), DurationTracker / Duration);
	const float NewDilation = FMath::InterpEaseIn(CurrentSlomoRate, 1.0f, DurationTracker / Duration, 2.5f);
	//UE_LOG(LogTemp, Warning, TEXT("New dilation: %f"), NewDilation);
	RealSlomoMechanic(NewDilation);

	if (NewDilation >= 1)
	{
		StopSlomo();
		GetWorld()->GetTimerManager().ClearTimer(SlomoHandle);
		DurationTracker = 0;
	}
	else
	{
		// Continue
		const FTimerDelegate TimerDel = FTimerDelegate::CreateUObject(this, &UEmphasisSubsystem::ReturningToNormalTime, Duration);
		GetWorld()->GetTimerManager().SetTimer(SlomoHandle, TimerDel, SlomoTickRate * NewDilation, true);
	}
}

void UEmphasisSubsystem::RealSlomoMechanic(const float InRate) const
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), InRate);

	float LowestPitch = 0.75f;
	if (OverridenGlobalPitch > 0.0f)
	{
		LowestPitch = OverridenGlobalPitch;
	}

	const float Pitch = FMath::GetMappedRangeValueClamped<float, float>({0.25f, 1.0f}, {LowestPitch, 1.0f}, InRate);
	UGameplayStatics::SetGlobalPitchModulation(GetWorld(), Pitch, 0.0f);
}

void UEmphasisSubsystem::BeginHitStop(AActor* Caller, AActor* Target, const float Duration)
{
	Caller->CustomTimeDilation = 0.0f;
	Target->CustomTimeDilation = 0.0f;

	FTimerHandle TimerHandle;
	const FTimerDelegate TimerDel = FTimerDelegate::CreateUObject(this, &UEmphasisSubsystem::FinishHitStop, Caller, Target);
	GetTimerManager().SetTimer(TimerHandle, TimerDel, Duration, false);
}

void UEmphasisSubsystem::FinishHitStop(AActor* Caller, AActor* Target)
{
	if (HitStoppingActors.Contains(Caller))
	{
		Caller->CustomTimeDilation = 1.0f;
		HitStoppingActors.Remove(Caller);
	}
	if (HitStoppingActors.Contains(Target))
	{
		Target->CustomTimeDilation = 1.0f;
		HitStoppingActors.Remove(Target);
	}
}

FTimerManager& UEmphasisSubsystem::GetTimerManager() const
{
	return GetWorld()->GetTimerManager();
}
