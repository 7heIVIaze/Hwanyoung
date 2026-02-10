// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "HwanyoungAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API UHwanyoungAssetManager : public UAssetManager
{
	GENERATED_BODY()

public: 
	virtual void StartInitialLoading() override;
};
