// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYWindProjectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"
#include "HYPoolSubSystem.h"
#include "Character/HYAggroNPCBase.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AHYWindProjectile::AHYWindProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;
	Box->OnComponentBeginOverlap.AddDynamic(this, &AHYWindProjectile::OnComponentBeginOverlap);

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
void AHYWindProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AHYWindProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHYWindProjectile::ShootProjectile()
{
	Super::ShootProjectile();
#pragma region Teammates Code
	if (bIsRandom)
	{
		Speed = FMath::RandRange(MinSpeed, MaxSpeed);
	}

	if (IsValid(TargetActor))
	{
		FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(),
			TargetActor->GetActorLocation());

		GetProjectileMovement()->Velocity = Direction * Speed;
	}
#pragma endregion
}

void AHYWindProjectile::InitializeActor(AActor* _Owner, APawn* _Instigator, AActor* _TargetActor, float _Gravity, float _Lifetime, float _MinSpeed, float _MaxSpeed, bool _IsRandom)
{
	TargetActor = _TargetActor;
	bIsRandom = _IsRandom;
	MinSpeed = _MinSpeed;
	MaxSpeed = _MaxSpeed;
	
	InitializeProjectile(_Owner, _Instigator, Speed, _Gravity, _Lifetime);
}

void AHYWindProjectile::OnSpawnFromPool_Implementation()
{
	Super::OnSpawnFromPool();
}

void AHYWindProjectile::OnReturnToPool_Implementation()
{
	Super::OnReturnToPool();
}