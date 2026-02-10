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

#pragma region Functions
	// Update player transform in level to save data
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerTransformInLevel(FName _LevelName, FTransform _PlayerTransform);

	// Update honbaek amount to save data
	UFUNCTION(BlueprintCallable)
	void UpdateHonbaekAmount(int32 _Amount);

	// Update inventory items to save data
	UFUNCTION(BlueprintCallable)
	void UpdateInventoryItems(TArray<FInventoryItem> _InventoryItem);

	// Update lore data to save data
	UFUNCTION(BlueprintCallable)
	void UpdateLore(FName _BossName, FLore _LoreData);

	// Update checked tutorial to save data
	UFUNCTION(BlueprintCallable)
	void UpdateTutorialChecked(FName _TutorialName, bool _IsChecked);

	// Update buff items to save data
	UFUNCTION(BlueprintCallable)
	void UpdateBuffItemList(TArray<FInventoryItem> _BuffItemLists);

	// Update Journal to save data
	UFUNCTION(BlueprintCallable)
	void UpdateJournal(FName _JournalRowID);

	// Initialize character name and get its save data
	UFUNCTION(BlueprintCallable)
	void GetCharacterSaveData(FName _CharacterName);

	UFUNCTION(BlueprintCallable)
	void LoadSaveFile(FString _Filename, int32 _UserIndex);

	UFUNCTION(BlueprintCallable)
	bool SaveFileToSlot();
#pragma endregion

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
