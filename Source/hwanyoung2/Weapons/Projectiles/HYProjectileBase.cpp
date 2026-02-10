// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYProjectileBase.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"
#include "HYPoolSubSystem.h"
#include "Character/HYAggroNPCBase.h"

// Sets default values
AHYProjectileBase::AHYProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;
	Box->OnComponentBeginOverlap.AddDynamic(this, &AHYProjectileBase::OnComponentBeginOverlap);

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
void AHYProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called every frame
void AHYProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHYProjectileBase::ShootProjectile()
{
	
}

void AHYProjectileBase::InitializeProjectile(AActor* _Owner, APawn* _Instigator, float _Speed, float _Gravity, float _Lifetime)
{
	SetOwner(_Owner);
	SetInstigator(_Instigator);

	Speed = _Speed;
	Gravity = _Gravity;
	LifeTime = _Lifetime;

	GetWorldTimerManager().SetTimer(ReturnTimer, this, &AHYProjectileBase::LifeSpanEnded, LifeTime, false);

	// Ignore the owner
	Box->IgnoreActorWhenMoving(GetOwner(), true);
	ProjectileMovementComp->Activate(true);
}

void AHYProjectileBase::OnSpawnFromPool_Implementation()
{
	GetWorldTimerManager().ClearTimer(ReturnTimer);

	Super::OnSpawnFromPool();
}

void AHYProjectileBase::OnReturnToPool_Implementation()
{
	// Deactivate projectile movement
	ProjectileMovementComp->Deactivate();

	Super::OnReturnToPool();
}


void AHYProjectileBase::OnComponentBeginOverlap(UPrimitiveComponent* _OverlappedComp, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherIndexBody, bool _FromSweep, const FHitResult& _SweepResult)
{
	// If the hit actor is not AggroNPCBase or self, do.
	if (!_OtherActor->IsA<AHYAggroNPCBase>() && !_OtherActor->IsA<AHYProjectileBase>())
	{
		OnProjectileImpact.Broadcast(_OtherActor, _SweepResult);

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactVFX, _SweepResult.Location,
			FRotator(0, 0, 0), FVector(1.0f, 1.0f, 1.0f), true);
		
		GetWorldTimerManager().ClearTimer(ReturnTimer);

		LifeSpanEnded();
	}
}

void AHYProjectileBase::LifeSpanEnded()
{
	UHYPoolSubSystem* PoolSubSystem = GetGameInstance()->GetSubsystem<UHYPoolSubSystem>();
	PoolSubSystem->ReturnToPool(this);
}