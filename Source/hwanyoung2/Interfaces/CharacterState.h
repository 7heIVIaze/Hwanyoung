// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataAssets/CharacterStatusType.h"
#include "CharacterState.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UCharacterState : public UInterface
{
	GENERATED_BODY()
};

class HWANYOUNG2_API ICharacterState
{
	GENERATED_BODY()

public:

	virtual int32 SetCharacterState(ECharacterState _InCharacterState, bool _IsAddMode) = 0;

	virtual int32 GetCharacterState() = 0;
};
