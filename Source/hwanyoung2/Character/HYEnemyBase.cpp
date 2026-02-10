// Copyright 2024. TEAM DAON. All rights reserved.


#include "HYEnemyBase.h"

// Sets default values
AHYEnemyBase::AHYEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHYEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHYEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHYEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

