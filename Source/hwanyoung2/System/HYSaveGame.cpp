// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYSaveGame.h"
#include "Inventory/InventoryItem.h"


void UHYSaveGame::SetSaveData(FName _CharacterName, FSaveData _SaveData)
{
	SaveData[_CharacterName] = _SaveData;
}

void UHYSaveGame::SetPlayerTransformInLevel(FName _CharacterName, FName _LevelName, FTransform _PlayerTransform)
{
	SaveData[_CharacterName].SavedLevelTransform[_LevelName] = _PlayerTransform;
}

void UHYSaveGame::SetCurrency(FName _CharacterName, int32 _Currency)
{
	SaveData[_CharacterName].HonbaekAmount = _Currency;
}

void UHYSaveGame::SetInventoryItem(FName _CharacterName, TArray<FInventoryItem> _Inventory)
{
	SaveData[_CharacterName].SavedInventoryItem = _Inventory;
}

void UHYSaveGame::SetLoreData(FName _CharacterName, TMap<FName, FLore> _LoreData)
{
	SaveData[_CharacterName].LoreData = _LoreData;
}

void UHYSaveGame::SetCheckTutorial(FName _CharacterName, TMap<FName, bool> _IsTutorialChecked)
{
	SaveData[_CharacterName].TutorialChecked = _IsTutorialChecked;
}

