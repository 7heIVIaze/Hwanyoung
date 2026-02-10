// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Interactables/Items/HYManualPickUp.h"
#include "CraftingInfo.h"
#include "InventoryItem.generated.h"

/**
 * Represents an item that can be added to player's inventory
 */
USTRUCT(BlueprintType)
struct FInventoryItem : public FTableRowBase
{
	GENERATED_BODY();
public:

	FInventoryItem();

	//unique ID for the item
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	//queue of items that are dropped and spawned back into the world
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AHYManualPickUp> ItemPickup;

	//name of the item
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemName;

	//weight of the item
	//(this may not be important, depends on how we re-design the inventory)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemWeight;

	//value of the item 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemValue;

	//is the item only available through drops?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool OnlyDropped;

	//can this item be consumed/used?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanBeUsed;

	//can this item be bought at the store?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanBeBought;

	//is this a crafting item?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsCraftItem;

	//is this a cooking item?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsCookingItem;

	//is this a buff item?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsBuffItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemDescription;

	////all the possible crafting combinations for this particular item
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCraftingInfo> CraftCombinations;

	// The limit on the number of trade possible.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemTradeLimitNumber;

	bool operator==(const FInventoryItem& OtherItem) const {
		return ItemID == OtherItem.ItemID;
	}

	friend uint32 GetTypeHash(const FInventoryItem& InventoryItem) {

		return HashCombine(GetTypeHash(InventoryItem.ItemID), InventoryItem.ItemValue);

	}
};