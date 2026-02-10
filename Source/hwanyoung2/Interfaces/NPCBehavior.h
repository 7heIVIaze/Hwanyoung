// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataAssets/CharacterStatusType.h"
#include "DataAssets/CombatTypes.h"
#include "NPCBehavior.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UNPCBehavior : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HWANYOUNG2_API INPCBehavior
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Set NPC speed per each state.
	virtual float SetMovementSpeed(ENPCMovementSpeed _MovementType) = 0;

	// Get Patrol Route
	virtual AActor* GetPatrolRoute() = 0;

	// Get ideal attack/defense radius
	virtual void GetIdealRange(float& AttackRadius, float& DefenseRadius) = 0;

	virtual void Attack(AActor* _AttackTarget) = 0;

	// Get attack token for group battle
	virtual bool AttackStart(AActor* _AttackTarget, int _TokensNeeded) = 0;

	// Restore token
	virtual void AttackEnd(AActor* _AttackTarget) = 0;

	virtual void EquipWeapon() = 0;

	virtual void UnequipWeapon() = 0;

	virtual void StoreAttackTokens(AActor* _AttackTarget, int _Amount) = 0;

	virtual bool ChangeDefaultAttackMode(EDefaultAttackMode _IncommingMode) = 0;
};
