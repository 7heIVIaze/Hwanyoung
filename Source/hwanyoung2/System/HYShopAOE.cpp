// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYShopAOE.h"
#include "HYGameStateBase.h"
#include "Engine/DataTable.h"

// Sets default values
AHYShopAOE::AHYShopAOE()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHYShopAOE::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHYShopAOE::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AHYShopAOE::RandomizeTradableItemSets(int CurrentHonbaekAmount)
{
	AHYGameStateBase* GameState = Cast<AHYGameStateBase>(GetWorld()->GetGameState());

	if(GameState == nullptr)
	{
		return false;
	}

	UDataTable* ItemDataTable = GameState->GetItemDatabase();

	if (ItemDataTable == nullptr)
	{
		return false;
	}

	TArray<FInventoryItem*> ItemData;
	ItemDataTable->GetAllRows<FInventoryItem>("", ItemData);

	if (ItemData.Num() == 0)
	{
		return false;
	}

	// Item count
	int count = 0;
	for (auto Item : ItemData)
	{
		// If the item is not only dropped and item value is less and equal than honbaek amount and over than 0, and can be bought.
		if (!Item->OnlyDropped && Item->ItemValue > 0
			&& Item->ItemValue <= CurrentHonbaekAmount && Item->CanBeBought)
		{
			++count;
			// Add item to final items since the final items are 5.
			if (FinalItems.Num() < ItemNumber)
			{
				FinalItems.Add(*Item);
			}
			else
			{
				int r = FMath::RandRange(0, count - 1);
				if (r < ItemNumber)
				{
					FinalItems[r] = *Item;
				}
			}
		}
	}

	if (FinalItems.Num() == ItemNumber)
	{
		return true;
	}
	return false;
}

TMap<FInventoryItem, int> AHYShopAOE::GetTradableItemSets(int CurrentHonbaekAmount)
{
	TMap<FInventoryItem, int> ResultItems;
	//TArray<FInventoryItem> TempItems = FinalItems;

	for (FInventoryItem Item : FinalItems)
	{
		int MaxAmount = CurrentHonbaekAmount / Item.ItemValue;
		int Amount = Item.ItemTradeLimitNumber; //FMath::Clamp(FMath::RandRange(0, MaxAmount), 1, TempItem.ItemTradeLimitNumber);

		ResultItems.Add({ Item, Amount });
	}

	return ResultItems;
}