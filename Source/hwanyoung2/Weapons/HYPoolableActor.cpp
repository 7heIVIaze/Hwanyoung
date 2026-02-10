// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYPoolableActor.h"

// Sets default values
AHYPoolableActor::AHYPoolableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHYPoolableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHYPoolableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHYPoolableActor::OnSpawnFromPool_Implementation()
{
	bIsActive = true;
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}

void AHYPoolableActor::OnReturnToPool_Implementation()
{
	bIsActive = false;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

}

