// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/RotationLibrary.h"

FQuat URotationLibrary::Lerp(const FQuat& A, const FQuat& B, const float& Alpha)
{
    return FMath::Lerp(A, B, Alpha);
}

FQuat URotationLibrary::FastLerp(const FQuat& A, const FQuat& B, const float& Alpha)
{
    return FQuat::FastLerp(A, B, Alpha);
}

FQuat URotationLibrary::BiLerp(const FQuat& P00, const FQuat& P10, const FQuat& P01, const FQuat& P11, const float & FracX, const float & FracY)
{
    return FMath::BiLerp(P00, P10, P01, P11, FracX, FracY);
}

FQuat URotationLibrary::FastBiLerp(const FQuat& P00, const FQuat& P10, const FQuat& P01, const FQuat& P11, const float & FracX, const float & FracY)
{
    return FQuat::FastBilerp(P00, P10, P01, P11, FracX, FracY);
}

FQuat URotationLibrary::Slerp(const FQuat& A, const FQuat& B, const float & Slerp)
{
    return FQuat::Slerp(A, B, Slerp);
}

FQuat URotationLibrary::Slerp_NotNormalized(const FQuat& A, const FQuat& B, const float & Slerp)
{
    return FQuat::Slerp_NotNormalized(A, B, Slerp);
}

float URotationLibrary::Dot(const FQuat& A, const FQuat& B)
{
    return A | B;
}

FQuat URotationLibrary::VectorToOrientationQuat(const FVector Vector)
{
    return Vector.ToOrientationQuat();
}

FRotator URotationLibrary::VectorToOrientationRotator(const FVector Vector)
{
    return Vector.ToOrientationRotator();
}

FQuat URotationLibrary::RotatorToQuat(const FRotator Rotator)
{
    return Rotator.Quaternion();
}

FRotator URotationLibrary::QuatToRotator(const FQuat& Quat)
{
    return FRotator(Quat);
}

float URotationLibrary::AngleBetweenDirectionVectorsRad(FVector A, FVector B)
{
    A.Normalize(1.0f);
    B.Normalize(1.0f);

    return FGenericPlatformMath::Acos(FVector::DotProduct(A, B));
}

float URotationLibrary::AngleBetweenDirectionVectorsDeg(FVector A, FVector B)
{
    A.Normalize(1.0f);
    B.Normalize(1.0f);

    return FMath::RadiansToDegrees(FGenericPlatformMath::Acos(FVector::DotProduct(A, B)));
}

float URotationLibrary::AngleBetweenQuatsRad(FQuat A, FQuat B)
{
    return A.AngularDistance(B);
}

float URotationLibrary::AngleBetweenQuatsDeg(FQuat A, FQuat B)
{
    return FMath::RadiansToDegrees(A.AngularDistance(B));
}

FQuat URotationLibrary::EulerDegToQuat(const FVector Euler)
{
    return FQuat::MakeFromEuler(Euler);
}

FVector URotationLibrary::QuatToEulerDeg(const FQuat& Quat)
{
    return Quat.Euler();
}

FQuat URotationLibrary::QuatPlusQuat(const FQuat& A, const FQuat& B)
{
    return A + B;
}

FQuat URotationLibrary::QuatMinusQuat(const FQuat& A, const FQuat& B)
{
    return A - B;
}

FQuat URotationLibrary::QuatMultiplyQuat(const FQuat& A, const FQuat& B)
{
    return A * B;
}

FQuat URotationLibrary::QuatMultiplyFloatScale(const FQuat& A, const float B)
{
    return A * B;
}

FQuat URotationLibrary::QuatDivFloatScale(const FQuat& A, const float B)
{
    return A / B;
}

bool URotationLibrary::AreQuatsEqual(const FQuat& A, const FQuat& B)
{
    return A == B;
}

FString URotationLibrary::QuatToString(const FQuat& Quat)
{
    return FString::Printf(TEXT("X=%f,Y=%f,Z=%f,W=%f"), Quat.X, Quat.Y, Quat.Z, Quat.W);
}

void URotationLibrary::AddActorLocalRotation(AActor* Actor, const FQuat& Quat, bool bSweep, FHitResult& OutSweepHitResult, bool bTeleport)
{
    ETeleportType Teleport;

    if (bTeleport)
    {
        Teleport = ETeleportType::TeleportPhysics;
    }
    else
    {
        Teleport = ETeleportType::None;
    }

    Actor->AddActorLocalRotation(Quat, bSweep, &OutSweepHitResult, Teleport);
}

void URotationLibrary::AddActorWorldRotation(AActor* Actor, const FQuat& Quat, bool bSweep, FHitResult& OutSweepHitResult, bool bTeleport)
{
    ETeleportType Teleport;

    if (bTeleport)
    {
        Teleport = ETeleportType::TeleportPhysics;
    }
    else
    {
        Teleport = ETeleportType::None;
    }

    Actor->AddActorWorldRotation(Quat, bSweep, &OutSweepHitResult, Teleport);
}

void URotationLibrary::AddComponentLocalRotation(USceneComponent* Component, const FQuat& Quat, bool bSweep, FHitResult& OutSweepHitResult, bool bTeleport)
{
    ETeleportType Teleport;

    if (bTeleport)
    {
        Teleport = ETeleportType::TeleportPhysics;
    }
    else
    {
        Teleport = ETeleportType::None;
    }

    Component->AddLocalRotation(Quat, bSweep, &OutSweepHitResult, Teleport);
}

void URotationLibrary::AddComponentRelativeRotation(USceneComponent* Component, const FQuat& Quat, bool bSweep, FHitResult& OutSweepHitResult, bool bTeleport)
{
    ETeleportType Teleport;

    if (bTeleport)
    {
        Teleport = ETeleportType::TeleportPhysics;
    }
    else
    {
        Teleport = ETeleportType::None;
    }

    Component->AddRelativeRotation(Quat, bSweep, &OutSweepHitResult, Teleport);
}

void URotationLibrary::AddComponentWorldRotation(USceneComponent* Component, const FQuat& Quat, bool bSweep, FHitResult& OutSweepHitResult, bool bTeleport)
{
    ETeleportType Teleport;

    if (bTeleport)
    {
        Teleport = ETeleportType::TeleportPhysics;
    }
    else
    {
        Teleport = ETeleportType::None;
    }

    Component->AddWorldRotation(Quat, bSweep, &OutSweepHitResult, Teleport);
}

void URotationLibrary::SetActorRelativeRotation(AActor* Actor, const FQuat& Quat, bool bSweep, FHitResult& OutSweepHitResult, bool bTeleport)
{
    ETeleportType Teleport;

    if (bTeleport)
    {
        Teleport = ETeleportType::TeleportPhysics;
    }
    else
    {
        Teleport = ETeleportType::None;
    }

    Actor->SetActorRelativeRotation(Quat, bSweep, &OutSweepHitResult, Teleport);
}

void URotationLibrary::SetActorRotation(AActor* Actor, const FQuat& Quat, bool bTeleport)
{
    ETeleportType Teleport;

    if (bTeleport)
    {
        Teleport = ETeleportType::TeleportPhysics;
    }
    else
    {
        Teleport = ETeleportType::None;
    }

    Actor->SetActorRotation(Quat, Teleport);
}

void URotationLibrary::SetComponentRelativeRotation(USceneComponent* Component, const FQuat& Quat, bool bSweep, FHitResult& OutSweepHitResult, bool bTeleport)
{
    ETeleportType Teleport;

    if (bTeleport)
    {
        Teleport = ETeleportType::TeleportPhysics;
    }
    else
    {
        Teleport = ETeleportType::None;
    }

    Component->SetRelativeRotation(Quat, bSweep, &OutSweepHitResult, Teleport);
}

void URotationLibrary::SetComponentWorldRotation(USceneComponent* Component, const FQuat& Quat, bool bSweep, FHitResult& OutSweepHitResult, bool bTeleport)
{
    ETeleportType Teleport;

    if (bTeleport)
    {
        Teleport = ETeleportType::TeleportPhysics;
    }
    else
    {
        Teleport = ETeleportType::None;
    }

    Component->SetWorldRotation(Quat, bSweep, &OutSweepHitResult, Teleport);
}

FQuat URotationLibrary::GetTransformQuat(const FTransform Transform)
{
    return Transform.GetRotation();
}

void URotationLibrary::BreakTransformQuat(const FTransform Transform, FVector& Location, FQuat& RotationQuat, FVector& Scale)
{
    Location = Transform.GetLocation();
    RotationQuat = Transform.GetRotation();
    Scale = Transform.GetScale3D();
}

void URotationLibrary::QuatAxisAngleRad(const FQuat& Quat, float& OutAngleRad, FVector& OutAxis)
{
    OutAngleRad = Quat.GetAngle();
    OutAxis = Quat.GetRotationAxis();
}

void URotationLibrary::QuatAxisAngleDeg(const FQuat& Quat, float& OutAngleDeg, FVector& OutAxis)
{
    OutAngleDeg = FMath::RadiansToDegrees(Quat.GetAngle());
    OutAxis = Quat.GetRotationAxis();
}

FQuat URotationLibrary::FindBetweenNormals(const FVector& Normal1, const FVector& Normal2)
{
    return FQuat::FindBetweenNormals(Normal1, Normal2);
}

FVector URotationLibrary::GetForwardVector(const FQuat& Quat)
{
    return Quat.GetForwardVector();
}

FVector URotationLibrary::GetRightVector(const FQuat& Quat)
{
    return Quat.GetRightVector();
}

FVector URotationLibrary::GetUpVector(const FQuat& Quat)
{
    return Quat.GetUpVector();
}

FQuat URotationLibrary::Inverse(const FQuat& Quat)
{
    return Quat.Inverse();
}

FVector URotationLibrary::QuatToVector(const FQuat& Quat)
{
    return Quat.Vector();
}

FRotator URotationLibrary::RotInterpByQuat(const FRotator& Current, const FRotator& Target, const float DelaTime, const float InterpSpeed)
{
    const FQuat Quat = FMath::QInterpTo(Current.Quaternion(), Target.Quaternion(), DelaTime, InterpSpeed);
    return Quat.Rotator();
}