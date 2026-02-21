// // Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HYSaveManagerSusbsystem.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API UHYSaveManagerSusbsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SavePuzzleCompleted(FName PuzzleId);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UHYSaveGame* SaveGame;
};
