// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "HYSaveGame.h"
#include "HYGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API UHYGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;

#pragma region Getter
	UFUNCTION(BlueprintCallable)
	TMap<FName, FLore> GetLoreData() { return SaveData.LoreData; }

	UFUNCTION(BlueprintCallable)
	TMap<FName, bool> GetCheckTutorial() { return SaveData.TutorialChecked; }
#pragma endregion	

public:
#pragma region Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
	TObjectPtr<class UHYSaveGame> SaveGameSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
	FSaveData SaveData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
	FName PlayerCharacterName;

	// Current level name for async loading
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Current Level")
	FString TargetLevelName;
#pragma endregion
};
