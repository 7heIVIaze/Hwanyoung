// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYSaveGame.h"

#include "Inventory/InventoryItem.h"
#include "Teammates/HYQuestManager.h"

void UHYSaveGame::SetSaveData(FName CharacterName, FSaveData NewSaveData)
{
	SaveData.FindOrAdd(CharacterName) = NewSaveData;
}

void UHYSaveGame::SaveLoreDataOnly(TMap<FName, FLore> Lore, FName CharacterName)
{
	for (auto& Pair : Lore)
	{
		SaveData.FindOrAdd(CharacterName).LoreData.Add(Pair.Key, Pair.Value);
	}
}

void UHYSaveGame::SetPlayerTransformInLevel(FName CharacterName, FName LevelName, FTransform PlayerTransform)
{
	SaveData.FindOrAdd(CharacterName).SavedLevelTransform.FindOrAdd(LevelName) = PlayerTransform;
}

void UHYSaveGame::SetCurrency(FName CharacterName, int32 Currency)
{
	SaveData.FindOrAdd(CharacterName).HonbaekAmount = Currency;
}

void UHYSaveGame::SetInventoryItem(FName CharacterName, TArray<FInventoryItem> Inventory)
{
	SaveData.FindOrAdd(CharacterName).SavedInventoryItem = Inventory;
}

void UHYSaveGame::SetLoreData(FName CharacterName, TMap<FName, FLore> LoreData)
{
	SaveData.FindOrAdd(CharacterName).LoreData = LoreData;
}

void UHYSaveGame::SetCheckTutorial(FName CharacterName, TMap<FName, bool> IsTutorialChecked)
{
	SaveData.FindOrAdd(CharacterName).TutorialChecked = IsTutorialChecked;
}

void UHYSaveGame::UpdateClearedPuzzleData(FName CharacterName, FName ClearedPuzzleId)
{
	SaveData.FindOrAdd(CharacterName).ClearedPuzzle.FindOrAdd(ClearedPuzzleId);
}

void UHYSaveGame::UpdateAffinityData(FName CharacterName, FName NPCName, FAffinityData AffinityAmount)
{
	FAffinityData AffinityLevel = SaveData.FindOrAdd(CharacterName).Affinity.FindOrAdd(NPCName);

	AffinityLevel.AffinityLevel = AffinityAmount.AffinityLevel; // Level
	AffinityLevel.AffinityPoint = AffinityAmount.AffinityPoint; // Point
}

