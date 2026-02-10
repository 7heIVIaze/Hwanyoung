// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ManaUser.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UManaUser : public UInterface
{
	GENERATED_BODY()
};

class HWANYOUNG2_API IManaUser
{
	GENERATED_BODY()

public:
	// Replenish Mana
	virtual float ReplenishMana(float _AmountOfMana) = 0;

	// Consume Mana
	virtual bool UseMana(float _ManaToBeUsed) = 0;

	virtual float GetCurrentMana() = 0;

	virtual float GetMaxMana() = 0;

	virtual float GetCurrentUltPoints() = 0;

	virtual float GetMaxUltPoints() = 0;

	// Consume ultimate points
	virtual bool UseUltimatePoints(float _UsedUltPoints) = 0;

	// Replenish ultimate points
	virtual float AddUltimatePoints(float _AddedUltimatePoints) = 0;

	// Is MP Recharge passive activated?
	virtual bool IsActivatedMPRecharge() = 0;
};
