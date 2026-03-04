// // Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "HYTreasureChest.h"
#include "HYWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API AHYWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public: 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelData")
	ELevelID LevelIndex;
};
