// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataAssets/CombatTypes.h"
#include "Damagable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UDamagable : public UInterface
{
	GENERATED_BODY()
};

class HWANYOUNG2_API IDamagable
{
	GENERATED_BODY()

public:
	// Heal Character
	virtual float Heal(float _AmountToHeal) = 0;

	// Deal Damage
	// @param _DamageInfo: Damage information structure, _DamageInstigator: Who deal damage, _DamageDuration: Duration of DOT effect, _DamageInterval: Interval between DOT ticks.
	virtual bool TakeDamage(FDamageInfo _DamageInfo, AActor* _DamageInstigator, float _DamageDuration, float _DamageInterval, const FHitResult& _Hit) = 0;

	// Check if the character is dead
	virtual bool IsDead() = 0;

	virtual float GetCurrentHP() = 0;

	virtual float GetMaxHP() = 0;

	// Check if the character is attacking
	virtual bool IsAttacking() = 0;

	// Reserve attack token for group battle while the mobs start attacking.
	// @param _Amount: the number of mobs who start attacking player.
	virtual bool ReserveAttackToken(int _Amount) = 0;

	// Return the token after the mobs attack
	virtual void ReturnAttackToken(int _Amount) = 0;

	// Is HP Recharge Passive activated?
	virtual bool IsActivatedHPRecharge() = 0;

	// Stop DOT Damage Ticks
	virtual void StopDOT() = 0;
};
