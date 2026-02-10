// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatSystem.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatSystem : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HWANYOUNG2_API ICombatSystem
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Toggle weapon trace
	virtual void ToggleWeaponTrace(bool _WeaponTraceOn) = 0;

	// Get weapon mesh
	virtual USkeletalMeshComponent* GetCharacterMesh() = 0;

};
