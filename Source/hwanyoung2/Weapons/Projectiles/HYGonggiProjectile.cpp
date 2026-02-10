// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYGonggiProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "HYPoolSubSystem.h"

AHYGonggiProjectile::AHYGonggiProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
}

void AHYGonggiProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (nullptr != SpeedCurveFloat)
	{
		FOnTimelineFloat SpeedFloat;
		SpeedFloat.BindDynamic(this, &AHYGonggiProjectile::RetrieveUpdated);
		RetrieveTimeline.AddInterpFloat(SpeedCurveFloat, SpeedFloat);

		FOnTimelineEvent TimelineFinished;
		TimelineFinished.BindDynamic(this, &AHYGonggiProjectile::RetrieveFinished);
		RetrieveTimeline.SetTimelineFinishedFunc(TimelineFinished);

		RetrieveTimeline.SetLooping(false);
	}
}

void AHYGonggiProjectile::Tick(float DeltaTime)
{
	if (RetrieveTimeline.IsPlaying())
	{
		RetrieveTimeline.TickTimeline(DeltaTime);
	}
}

void AHYGonggiProjectile::InitializeActor(AActor* _Owner, APawn* _Instigator, AActor* _TargetActor, bool _IsAimedAtTarget, float _Speed, float _Gravity, float _Lifetime)
{
	InitializeProjectile(_Owner, _Instigator, _Speed, _Gravity, _Lifetime);

	TargetActor = _TargetActor;
	bIsAimedAtTarget = _IsAimedAtTarget;
}

void AHYGonggiProjectile::ShootProjectile()
{
	Super::ShootProjectile();
	
	// If the target actor is valid, then fire
	if (IsValid(TargetActor))
	{
		// if it is aimed at the target, then fire to target
		if (bIsAimedAtTarget)
		{
			RotateToTarget();
		}
		// if not, then fire straight
		else
		{
			ShootStraight();
		}
	}
}

void AHYGonggiProjectile::RotateToTarget()
{
	FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), 
		TargetActor->GetActorLocation());
	
	GetProjectileMovement()->Velocity = Direction * Speed;
}

void AHYGonggiProjectile::ShootStraight()
{
	GetProjectileMovement()->Velocity = GetActorForwardVector() * Speed;
}


void AHYGonggiProjectile::OnSpawnFromPool_Implementation()
{
	Super::OnSpawnFromPool();

	Niagara->Activate();
}

void AHYGonggiProjectile::OnReturnToPool_Implementation()
{
	Super::OnReturnToPool();

	// Deactivate niagara component and make it invisible
	Niagara->Deactivate();
}

void AHYGonggiProjectile::RetrieveGonggi(AActor* _SourceActor, FVector _Location)
{
	// Check if the actor is valid
	if (IsValid(_SourceActor))
	{
#pragma region Teammates Codes
		ProjectileMovementComp->Velocity = FVector(0.0f);

		StartLocation = _Location;

		RetrieveTimeline.PlayFromStart();
#pragma endregion

	}
	else
	{
		// Return this projectile to the pool after 1 second.
		GetWorldTimerManager().SetTimer(ReturnTimer, this, &AHYGonggiProjectile::LifeSpanEnded, 1.0f, false);
	}
}

#pragma region Teammates Code
void AHYGonggiProjectile::RetrieveUpdated(float _Speed)
{
	AActor* SourceActor = GetOwner();
	FVector SourceLocation = SourceActor->GetActorLocation() +
		SourceActor->GetActorUpVector() * 80.0f;

	FVector ALocation = GetActorLocation();
	FVector BLocation = SourceLocation + StartLocation;

	SetActorLocation(FMath::Lerp(ALocation, BLocation, _Speed));
}
#pragma endregion

void AHYGonggiProjectile::RetrieveFinished()
{
	GetWorldTimerManager().ClearTimer(ReturnTimer);

	GetWorldTimerManager().SetTimer(ReturnTimer, this, &AHYGonggiProjectile::LifeSpanEnded, 0.5f, false);
}