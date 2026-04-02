// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYSaveManagerSusbsystem.h"

#include "HYSaveGame.h"
#include "Kismet/GameplayStatics.h"

void UHYSaveManagerSusbsystem::UpdatePlayerTransformInLevel(FName LevelName, FTransform PlayerTransform)
{
	SaveData.SavedLevelTransform.FindOrAdd(LevelName) = PlayerTransform;
}

void UHYSaveManagerSusbsystem::UpdateHonbaekAmount(int32 Amount)
{
	SaveData.HonbaekAmount = Amount;
}

void UHYSaveManagerSusbsystem::UpdateInventoryItems(TArray<FInventoryItem> InventoryItem)
{
	SaveData.SavedInventoryItem = InventoryItem;
}

void UHYSaveManagerSusbsystem::UpdateLore(FName BossName, FLore LoreData)
{
	SaveData.LoreData.FindOrAdd(BossName) = LoreData;
}

void UHYSaveManagerSusbsystem::UpdateTutorialChecked(FName TutorialName, bool IsChecked)
{
	SaveData.TutorialChecked.FindOrAdd(TutorialName) = IsChecked;
}

void UHYSaveManagerSusbsystem::UpdateBuffItemList(TArray<FInventoryItem> BuffItemLists)
{
	SaveData.SavedBuffItemList = BuffItemLists;
}

void UHYSaveManagerSusbsystem::UpdateJournal(FName JournalRowID)
{
	SaveData.Journal.Add(JournalRowID);
}

void UHYSaveManagerSusbsystem::UpdateBaseCampHaeTae(FVector FromLocation, FName MapName)
{
	SaveData.SavedBaseCampLocation.FindOrAdd(MapName) = FromLocation;
}

void UHYSaveManagerSusbsystem::GetCharacterSaveData(FName CharacterName)
{
	PlayerCharacterName = CharacterName;

	if (FSaveData* Data = SaveGame->GetSaveData().Find(PlayerCharacterName))
	{
		SaveData = *Data;
		return;
	}

	SaveData = FSaveData();
	SaveData.TutorialChecked.Add(TEXT("HowToCombat"), false);
	SaveData.TutorialChecked.Add(TEXT("HowToUseSkill"), false);
	SaveData.TutorialChecked.Add(TEXT("HowToCraft"), false);
	SaveData.TutorialChecked.Add(TEXT("HowToCook"), false);
	SaveData.TutorialChecked.Add(TEXT("HowToUseInventory"), false);
}

void UHYSaveManagerSusbsystem::LoadSaveFile(FString Filename, int32 UserIndex)
{
	SaveGame = Cast<UHYSaveGame>(UGameplayStatics::LoadGameFromSlot(Filename, UserIndex));
}

bool UHYSaveManagerSusbsystem::SaveFileToSlot()
{
	bool bResult = false;

	if (IsValid(SaveGame))
	{
		SaveGame->GetSaveData().Add(PlayerCharacterName, SaveData);
		bResult = UGameplayStatics::SaveGameToSlot(SaveGame, TEXT("save"), 0);
	}

	return bResult;
}


void UHYSaveManagerSusbsystem::SavePuzzleCompleted(FName PuzzleId)
{
	SaveGame->UpdateClearedPuzzleData(PlayerCharacterName, PuzzleId);
}
