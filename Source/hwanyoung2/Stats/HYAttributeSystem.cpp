// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYAttributeSystem.h"
#include "DataAssets/CombatTypes.h"

// Sets default values for this component's properties
UHYAttributeSystem::UHYAttributeSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UHYAttributeSystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHYAttributeSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UHYAttributeSystem::Buff(FStatBuffInfo AttributeInfo)
{
	// If this buff is based on factor
	if (AttributeInfo.bIsFactorBased)
	{
		MultipliedBuffFactor = MultipliedBuffFactor + AttributeInfo.AttributeSet;
	}
	// else if this buff is just based on numerical
	else
	{
		AddedBuffFactor = AddedBuffFactor + AttributeInfo.AttributeSet;
	}

	CalculateFinalStats();

	return true;
}

bool UHYAttributeSystem::ResetAttributes()
{
	CurrentPhysDMG = OriginalPhysDMG;
	CurrentCritRate = OriginalCritRate;
	CurrentCritDMG = OriginalCritDMG;
	CurrentDEF = OriginalDEF;
	
	return true;
}

bool UHYAttributeSystem::Debuff(FStatBuffInfo AttributeInfo)
{
	// If this buff is based on factor
	if (AttributeInfo.bIsFactorBased)
	{
		MultipliedDebuffFactor = MultipliedDebuffFactor + AttributeInfo.AttributeSet;
	}
	// else if this buff is just based on numerical
	else
	{
		AddedDebuffFactor = AddedDebuffFactor + AttributeInfo.AttributeSet;
	}

	CalculateFinalStats();
	return true;
}

bool UHYAttributeSystem::RemoveBuff(FStatBuffInfo AttributeInfo)
{
	// Do as debuff logic
	// If this buff is based on factor
	if (AttributeInfo.bIsFactorBased)
	{
		MultipliedBuffFactor = MultipliedBuffFactor - AttributeInfo.AttributeSet;
	}
	// else if this buff is just based on numerical
	else
	{
		AddedBuffFactor = AddedBuffFactor - AttributeInfo.AttributeSet;
	}

	CalculateFinalStats();

	return true;
}

bool UHYAttributeSystem::RemoveDebuff(FStatBuffInfo AttributeInfo)
{
	// Do as buff logic
	// If this buff is based on factor
	if (AttributeInfo.bIsFactorBased)
	{
		MultipliedDebuffFactor = MultipliedDebuffFactor - AttributeInfo.AttributeSet;
	}
	// else if this buff is just based on numerical
	else
	{
		AddedDebuffFactor = AddedDebuffFactor - AttributeInfo.AttributeSet;
	}

	CalculateFinalStats();

	return true;
}

void UHYAttributeSystem::CalculateFinalStats()
{
	// Calculate the multiplied factor
	FAttributeSetInfo MultipliedFactor = MultipliedBuffFactor - MultipliedDebuffFactor;
	MultipliedFactor.PhysicalDamage = FMath::Clamp(MultipliedFactor.PhysicalDamage, 0.0f, 4.0f);
	MultipliedFactor.CriticalRate = FMath::Clamp(MultipliedFactor.CriticalRate, 0.0f, 4.0f);
	MultipliedFactor.CriticalDamage = FMath::Clamp(MultipliedFactor.CriticalDamage, 0.0f, 4.0f);
	MultipliedFactor.Defense = FMath::Clamp(MultipliedFactor.Defense, 0.0f, 4.0f);

	// Calculate the added factor
	FAttributeSetInfo AddedFactor = AddedBuffFactor - AddedDebuffFactor;
	AddedFactor.PhysicalDamage = FMath::Clamp(AddedFactor.PhysicalDamage, -OriginalPhysDMG, 100000000.0);
	AddedFactor.CriticalRate = FMath::Clamp(AddedFactor.CriticalRate, -OriginalCritRate, 100.0f);
	AddedFactor.CriticalDamage = FMath::Clamp(AddedFactor.CriticalDamage, -OriginalCritDMG, 100000000.0);
	AddedFactor.Defense = FMath::Clamp(AddedFactor.Defense, -OriginalDEF, 100000000.0);


	// Do the final calculation of attribute set
	CurrentPhysDMG = FMath::Clamp(
		OriginalPhysDMG * (1 + MultipliedFactor.PhysicalDamage) + AddedFactor.PhysicalDamage,
		0.0f, 100000000.0f);

	CurrentCritRate = FMath::Clamp(
		CurrentCritRate * (1 + MultipliedFactor.CriticalRate) + AddedFactor.CriticalRate,
		0.0f, 100.0f);

	CurrentPhysDMG = FMath::Clamp(
		CurrentCritDMG * (1 + MultipliedFactor.CriticalDamage) + AddedFactor.CriticalDamage,
		0.0f, 100000000.0f);

	CurrentDEF = FMath::Clamp(
		OriginalPhysDMG * (1 + MultipliedFactor.Defense) + AddedFactor.Defense,
		0.0f, 100000000.0f);

}