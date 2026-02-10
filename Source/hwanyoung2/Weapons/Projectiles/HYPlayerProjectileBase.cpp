// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYPlayerProjectileBase.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"
#include "HYPoolSubSystem.h"
#include "Interfaces/Damagable.h"

// Sets default values
AHYPlayerProjectileBase::AHYPlayerProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;
	Box->OnComponentBeginOverlap.AddDynamic(this, &AHYPlayerProjectileBase::OnComponentBeginOverlap);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Box);

	TrailEffectLocation = CreateDefaultSubobject<USceneComponent>(TEXT("TrailEffectLocation"));
	TrailEffectLocation->SetupAttachment(Mesh);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComp->bAutoActivate = false;
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->ProjectileGravityScale = 0.5f;
}

// Called when the game starts or when spawned
void AHYPlayerProjectileBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AHYPlayerProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHYPlayerProjectileBase::Fire_Implementation(FVector _Direction, float _Strength)
{
	// Activate the projectile movement component
	ProjectileMovementComp->Activate();

	Strength = FMath::Lerp(MinSpeed, MaxSpeed, _Strength);
	ProjectileMovementComp->Velocity = _Direction * Strength;
	Velocity = ProjectileMovementComp->Velocity;

	ProjectileMovementComp->ProjectileGravityScale = FMath::Lerp(MinGravity, MaxGravity, _Strength);

	// Play sound
	UFMODBlueprintStatics::PlayEventAtLocation(this, ArrowSound, GetActorTransform(), true);

	// If the traill effect reference is valid, then reuse it
	if (TrailEffectRef)
	{
		TrailEffectRef->SetVisibility(true);
		TrailEffectRef->Activate(true);
	}
	// It not then spawn system
	else
	{
		TrailEffectRef = UNiagaraFunctionLibrary::SpawnSystemAttached(TrailEffect, TrailEffectLocation, TEXT("None"), 
			FVector(0, 0, 0), FRotator(0, 0, 0), EAttachLocation::KeepWorldPosition, true);
	}

	Box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Set timer to return to the pool
	GetWorldTimerManager().SetTimer(ReturnTimer, this, &AHYPlayerProjectileBase::LifeSpanEnded, 10.0f, false);
}

void AHYPlayerProjectileBase::OnSpawnFromPool_Implementation()
{
	Super::OnSpawnFromPool();
}

void AHYPlayerProjectileBase::OnReturnToPool_Implementation()
{
	// Deactivate projectile movement
	ProjectileMovementComp->Deactivate();

	// Deactivate, reset niagara component and make it invisible
	if (TrailEffectRef)
	{
		TrailEffectRef->Deactivate();
		TrailEffectRef->ResetSystem();
		TrailEffectRef->SetVisibility(false);
	}

	Super::OnReturnToPool();
}

void AHYPlayerProjectileBase::OnComponentBeginOverlap(UPrimitiveComponent* _OverlappedComp, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherIndexBody, bool _FromSweep, const FHitResult& _SweepResult)
{
	// If the hit actor is not instigator, do.
	if (GetInstigator() != _OtherActor)
	{
		OnArrowImpact.Broadcast(Strength, _OtherActor, _SweepResult);

		ProjectileMovementComp->Deactivate();
		Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		USkeletalMeshComponent* HitActorMesh = Cast<USkeletalMeshComponent>(_SweepResult.GetActor()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));

		// Check if the hit actor is dead
		UClass* ActorClass = _OtherActor->GetClass();
		bool bIsDead = false;
		if (ActorClass->ImplementsInterface(UDamagable::StaticClass()))
		{
			bIsDead = Cast<IDamagable>(_OtherActor)->IsDead();
		}

		if (bIsDead)
		{
			GetWorldTimerManager().ClearTimer(ReturnTimer);

			LifeSpanEnded();
		}
		else
		{
			if (nullptr != HitActorMesh)
			{
				// Attach to mesh
				AttachToComponent(HitActorMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true), HitActorMesh->FindClosestBone(_SweepResult.Location));
			}
			else
			{
				AttachToComponent(HitActorMesh, FAttachmentTransformRules::KeepWorldTransform, TEXT("None"));
			}
		
			UFMODBlueprintStatics::PlayEventAtLocation(this, HitSound, GetActorTransform(), true);

			GetWorldTimerManager().ClearTimer(ReturnTimer);
			GetWorldTimerManager().SetTimer(ReturnTimer, this, &AHYPlayerProjectileBase::LifeSpanEnded, 5.0f, false);
			//SetLifeSpan(10.0f);
		
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactVFX, _SweepResult.Location,
				FRotator(0, 0, 0), FVector(1.0f, 1.0f, 1.0f), true);

			if (_SweepResult.GetComponent()->IsSimulatingPhysics())
			{
				_SweepResult.GetComponent()->AddImpulse(Velocity / 200.0f);
			}

			FTimerHandle Timer;
			GetWorld()->GetTimerManager().SetTimer(Timer, this, &AHYPlayerProjectileBase::LifeSpanEnded, 1.0f, false);
		}
	}
}

