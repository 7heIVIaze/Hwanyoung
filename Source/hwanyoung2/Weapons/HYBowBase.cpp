// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYBowBase.h"

// Sets default values
AHYBowBase::AHYBowBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHYBowBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHYBowBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHYBowBase::DrawBegin_Implementation()
{
	BowState = EBowState::Draw;
}

void AHYBowBase::DrawEnd_Implementation()
{
	BowState = EBowState::Idle;
}
