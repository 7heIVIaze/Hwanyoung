// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYGameInstance.h"
#include "HYSaveGame.h"
#include "Inventory/InventoryItem.h"
#include "Kismet/GameplayStatics.h"

void UHYGameInstance::Init()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("save"), 0))
	{
		LoadSaveFile(TEXT("save"), 0);
	}
}

void UHYGameInstance::UpdatePlayerTransformInLevel(FName LevelName, FTransform PlayerTransform)
{
	SaveData.SavedLevelTransform[LevelName] = PlayerTransform;
}

void UHYGameInstance::UpdateHonbaekAmount(int32 Amount)
{
	SaveData.HonbaekAmount = Amount;
}

void UHYGameInstance::UpdateInventoryItems(TArray<FInventoryItem> InventoryItem)
{
	SaveData.SavedInventoryItem = InventoryItem;
}

void UHYGameInstance::UpdateLore(FName BossName, FLore LoreData)
{
	SaveData.LoreData[BossName] = LoreData;
}

void UHYGameInstance::UpdateTutorialChecked(FName TutorialName, bool IsChecked)
{
	SaveData.TutorialChecked[TutorialName] = IsChecked;
}

void UHYGameInstance::UpdateBuffItemList(TArray<FInventoryItem> BuffItemLists)
{
	SaveData.SavedBuffItemList = BuffItemLists;
}

void UHYGameInstance::UpdateJournal(FName JournalRowID)
{
	SaveData.Journal.Add(JournalRowID);
}

void UHYGameInstance::GetCharacterSaveData(FName CharacterName)
{
	PlayerCharacterName = CharacterName;

	if (SaveGameSystem->GetSaveData().Contains(PlayerCharacterName))
	{
		SaveData = *SaveGameSystem->GetSaveData().Find(PlayerCharacterName);
		return;
	}

	SaveData = FSaveData();
	SaveData.TutorialChecked.Add(TEXT("HowToCombat"), false);
	SaveData.TutorialChecked.Add(TEXT("HowToUseSkill"), false);
	SaveData.TutorialChecked.Add(TEXT("HowToCraft"), false);
	SaveData.TutorialChecked.Add(TEXT("HowToCook"), false);
	SaveData.TutorialChecked.Add(TEXT("HowToUseInventory"), false);
}

void UHYGameInstance::LoadSaveFile(FString Filename, int32 UserIndex)
{
	SaveGameSystem = Cast<UHYSaveGame>(UGameplayStatics::LoadGameFromSlot(Filename, UserIndex));
}

bool UHYGameInstance::SaveFileToSlot()
{
	bool bResult = false;

	if (IsValid(SaveGameSystem))
	{
		SaveGameSystem->GetSaveData().Add(PlayerCharacterName, SaveData);
		bResult = UGameplayStatics::SaveGameToSlot(SaveGameSystem, TEXT("save"), 0);
	}

	return bResult;
}
