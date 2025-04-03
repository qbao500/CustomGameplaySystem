// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_EnemyAnnounceAttack.h"

#include "Subsystems/CombatSubsystem.h"

UANS_EnemyAnnounceAttack::UANS_EnemyAnnounceAttack()
{
#if WITH_EDITOR
	bShouldFireInEditor = false;
	NotifyColor = FColor::FromHex("FF6C6CFF");
#endif
}

void UANS_EnemyAnnounceAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	Owner->GetWorld()->GetSubsystem<UCombatSubsystem>()->StartDangerousAttack(MeshComp->GetOwner());
}

void UANS_EnemyAnnounceAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	Owner->GetWorld()->GetSubsystem<UCombatSubsystem>()->StopDangerousAttack(MeshComp->GetOwner());
}
