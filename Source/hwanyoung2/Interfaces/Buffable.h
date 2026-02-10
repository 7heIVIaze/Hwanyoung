// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataAssets/CharacterStatusType.h"
#include "Buffable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UBuffable : public UInterface
{
	GENERATED_BODY()
};

class HWANYOUNG2_API IBuffable
{
	GENERATED_BODY()

public:
	// Buff character
	virtual bool Buff(FBuffableInfo _BuffableInfo) = 0;
	
	// Debuff character
	virtual bool Debuff(FBuffableInfo _BuffableInfo) = 0;

	// Reset all attributes
	virtual bool ResetAttributes() = 0;

	// Buff Expired character
	virtual bool RemoveBuff(FBuffableInfo _BuffableInfo) = 0;

	// Debuff Expired character
	virtual bool RemoveDebuff(FBuffableInfo _BuffableInfo) = 0;

	// Remove all applied debuffs
	virtual bool Cleanse() = 0;
};
