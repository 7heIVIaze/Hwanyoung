// Copyright 2024. TEAM DAON. All rights reserved.


#include "InventoryItem.h"

FInventoryItem::FInventoryItem()
{
	this->ItemID = FName(TEXT("No ID"));
	this->ItemName = FText::FromString("No Name");
	this->ItemWeight = 1;
	this->ItemValue = 1;
	this->ItemDescription = FText::FromString("No Description");
}
