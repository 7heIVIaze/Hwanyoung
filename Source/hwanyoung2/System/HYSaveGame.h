// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "HYSaveGame.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FLore
{
	GENERATED_BODY()
	
public:
	// Name of the boss
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BossName;

	// Has already interacted with the boss?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasInteracted;

	// Has slain the boss?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasBeenSlain;

	// Has received reward?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bReceivedReward;

public:
	FLore()
		: BossName(TEXT(""))
		, bHasInteracted(false)
		, bHasBeenSlain(false)
		, bReceivedReward(false)
	{ }

	FLore(FName _BossName, bool _HasInteracted, bool _HasBeenSlain, bool _ReceivedReward)
		: BossName(_BossName)
		, bHasInteracted(_HasInteracted)
		, bHasBeenSlain(_HasBeenSlain)
		, bReceivedReward(_ReceivedReward)
	{ }
};

USTRUCT(BlueprintType)
struct FSaveData
{
	GENERATED_BODY()

public:
	// The amount of honbaek(Money)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Data")
	int32 HonbaekAmount;
	// Save data of inventory items
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Data")
	TArray<FInventoryItem> SavedInventoryItem;
	// Save data of trasform in level.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Data")
	TMap<FName, FTransform> SavedLevelTransform;
	// Save data of Lore(Boss Status)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Data")
	TMap<FName, FLore> LoreData;
	// Save data of checked tutorial
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Data")
	TMap<FName, bool> TutorialChecked;
	// BuffItemList
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Data")
	TArray<FInventoryItem> SavedBuffItemList;
	// Journal
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Data")
	TSet<FName> Journal;


public:
	FSaveData()
		: HonbaekAmount(0)
		, SavedInventoryItem(TArray<FInventoryItem>())
		, SavedLevelTransform(TMap<FName, FTransform>())
		, LoreData(TMap<FName, FLore>())
		, TutorialChecked(TMap<FName, bool>())
		, SavedBuffItemList(TArray<FInventoryItem>())
		, Journal(TSet<FName>())
	{
	}

	FSaveData(int _HonbaekAmount, TArray<FInventoryItem> _SavedInventory, TMap<FName, FTransform> _LevelTransform, 
		TMap<FName, FLore> _LoreData, TMap<FName, bool> _CheckedTutorial, TArray<FInventoryItem> _BuffItemList, TSet<FName> _Journal)
		: HonbaekAmount(_HonbaekAmount)
		, SavedInventoryItem(_SavedInventory)
		, SavedLevelTransform(_LevelTransform)
		, LoreData(_LoreData)
		, TutorialChecked(_CheckedTutorial)
		, SavedBuffItemList(_BuffItemList)
		, Journal(_Journal)
	{
	}
};

UCLASS()
class HWANYOUNG2_API UHYSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	// Getter
	UFUNCTION(BlueprintCallable)
	TMap<FName, FTransform> GetSavedLevelAndTransform(FName _CharacterName) { return SaveData[_CharacterName].SavedLevelTransform; }

	UFUNCTION(BlueprintCallable)
	int32 GetSavedCurrency(FName _CharacterName) { return SaveData[_CharacterName].HonbaekAmount; }

	UFUNCTION(BlueprintCallable)
	TArray<FInventoryItem> GetSavedInventoryItems(FName _CharacterName) { return SaveData[_CharacterName].SavedInventoryItem; }

	UFUNCTION(BlueprintCallable)
	TMap<FName, FLore> GetSavedLoreData(FName _CharacterName) { return SaveData[_CharacterName].LoreData; }

	UFUNCTION(BlueprintCallable)
	TMap<FName, bool> GetTutorialChecked(FName _CharacterName) { return SaveData[_CharacterName].TutorialChecked; }

	// Setter
	UFUNCTION(BlueprintCallable)
	void SetSaveData(FName _CharacterName, FSaveData _SaveData);

	UFUNCTION(BlueprintCallable)
	void SetPlayerTransformInLevel(FName _CharacterName, FName _LevelName, FTransform _PlayerTransform);

	UFUNCTION(BlueprintCallable)
	void SetCurrency(FName _CharacterName, int32 _Currency);

	UFUNCTION(BlueprintCallable)
	void SetInventoryItem(FName _CharacterName, TArray<FInventoryItem> _Inventory);

	UFUNCTION(BlueprintCallable)
	void SetLoreData(FName _CharacterName, TMap<FName, FLore> _LoreData);

	UFUNCTION(BlueprintCallable)
	void SetCheckTutorial(FName _CharacterName, TMap<FName, bool> _IsTutorialChecked);

#pragma region Getter
	UFUNCTION(BlueprintCallable)
	TMap<FName, FSaveData> GetSaveData() { return SaveData; }
#pragma endregion
	// Save Game to slot
	/*UFUNCTION(BlueprintCallable)
	virtual void SaveGameToSlot(FString _SlotName, int32 _UserIndex) override;*/

private:
	// Key: Character Name(Type), Value: Save Data
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save Data", meta = (AllowPrivateAccess = "true"))
	TMap<FName, FSaveData> SaveData;
};
