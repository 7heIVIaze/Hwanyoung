// Copyright 2024. TEAM DAON. All rights reserved.


#include "HYGameStateBase.h"

AHYGameStateBase::AHYGameStateBase()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> BP_ItemDataTable(
		TEXT("DataTable'/Game/Hwanyoung/Interactables/ItemDataTable.ItemDataTable'"));

	ItemDatabase = BP_ItemDataTable.Object;
}

UDataTable* AHYGameStateBase::GetItemDatabase() const
{
	return ItemDatabase;
}
