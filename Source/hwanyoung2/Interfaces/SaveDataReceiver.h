// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataAssets/HwanyoungType.h"
#include "System/HYSaveGame.h"
#include "SaveDataReceiver.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USaveDataReceiver : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HWANYOUNG2_API ISaveDataReceiver
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable)
	virtual enum ESaveApplyState ApplySaveData(FSaveData SaveData) = 0;
};
