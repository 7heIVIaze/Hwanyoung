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
