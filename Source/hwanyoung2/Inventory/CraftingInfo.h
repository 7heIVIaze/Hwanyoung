// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CraftingInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FCraftingInfo : public FTableRowBase
{
	GENERATED_BODY();

public:

	//unique ID of item that is being used to create the item
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemAID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemBID;

	//unique ID of item that is being used to create the item
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemCID;

	//unique ID of item that is being used to create the item
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemDID;

	//unique ID of the item that gets created
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ProductID;

	//do we want to destroy the component?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDestroyItemA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDestroyItemB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDestroyItemC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDestroyItemD;

	bool operator==(const FCraftingInfo& OtherItem) const {
		return ItemAID == OtherItem.ItemAID
			&& ItemBID == OtherItem.ItemBID
			&& ItemCID == OtherItem.ItemCID
			&& ItemDID == OtherItem.ItemDID;
	}


}; 