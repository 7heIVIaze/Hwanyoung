// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYMPSystem.h"

// Sets default values for this component's properties
UHYMPSystem::UHYMPSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHYMPSystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHYMPSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UHYMPSystem::ReplenishMana(float _AmountOfMana)
{
	CurrentMana = FMath::Clamp(CurrentMana + _AmountOfMana, 0.0f, MaxMana);

	OnManaReplenished.Broadcast();

	return CurrentMana;
}

bool UHYMPSystem::UseMana(float _ToBeConsumedMana)
{
	// Check if current mana is less than consumed mana
	if (CurrentMana < _ToBeConsumedMana)
	{
		return false;
	}

	CurrentMana = FMath::Clamp(CurrentMana - _ToBeConsumedMana, 0.0f, MaxMana);

	OnManaUsed.Broadcast();

	return true;
}

bool UHYMPSystem::UseUltimatePoints(float _ConsumedUltPoints)
{
	// Check if current ultimate points are less than consumed ultimate points
	if (CurrentUltimatePoint < _ConsumedUltPoints)
	{
		return false;
	}

	CurrentUltimatePoint = FMath::Clamp(CurrentUltimatePoint - _ConsumedUltPoints, 0.0f, MaxUltimatePoint);

	OnUltimatePointsUsed.Broadcast();

	return false;
}

float UHYMPSystem::AddUltimatePoints(float _AddedUltPoints)
{
	CurrentUltimatePoint = FMath::Clamp(CurrentUltimatePoint + _AddedUltPoints, 0.0f, MaxUltimatePoint);

	OnUltimatePointsAdded.Broadcast();

	return CurrentUltimatePoint;
}

bool UHYMPSystem::ReplenishStamina(float _AmountOfStamina)
{
	if (CurrentStamina == MaxStamina)
	{
		return true;
	}

	CurrentStamina = FMath::Clamp(CurrentStamina + _AmountOfStamina, 0.0f, MaxStamina);

	return (CurrentStamina == MaxStamina);
}

bool UHYMPSystem::UseStamina(float _ToBeConsumedStamina)
{
	if (CurrentStamina > 0.0f)
	{
		CurrentStamina = FMath::Clamp(CurrentStamina - _ToBeConsumedStamina, 0.0f, MaxStamina);
		return true;
	}

	return false;
}

bool UHYMPSystem::Buff(float _AmountToBuffed)
{
	// Increase max MP temporarily.
	MaxMana += _AmountToBuffed;

	// And increase(replenish) current MP.(that is permanant)
	ReplenishMana(_AmountToBuffed);

	return true;
}

bool UHYMPSystem::RemoveBuff(float _AmountToBuffed)
{
	// Decrease the increased amount of MP
	MaxMana -= _AmountToBuffed;

	// Adjust the current MP to be between 0 and max mp.
	CurrentMana = FMath::Clamp(CurrentMana, 0.0f, MaxMana);

	return true;
}
