// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYShopNPCActor.h"
#include "Blueprint/UserWidget.h"
#include "Player/HYPlayerCharacController.h"

void AHYShopNPCActor::Interact_Implementation(APlayerController* Controller)
{
	Super::Interact_Implementation(Controller);

	if (ShopWidget)
	{
		ShopWidget->RemoveFromParent();
		ShopWidget = nullptr;
	}
	else
	{
		ShopWidget = CreateWidget<UUserWidget>(Controller, ShopWidgetClass);
		ShopWidget->AddToViewport();
	}
}

void AHYShopNPCActor::BuyTradableItem_Implementation(APlayerController* Controller, const FInventoryItem ItemToBuy, const int Amount)
{
	int CurrentAmount = *TradableItems.Find(ItemToBuy);

	TradableItems[ItemToBuy] = CurrentAmount - Amount;

	AHYPlayerCharacController* PlayerController = Cast<AHYPlayerCharacController>(Controller);

	if (PlayerController)
	{
		for (int i = 0; i < Amount; ++i)
		{
			PlayerController->AddItemToInventoryByID(ItemToBuy.ItemID);
		}
	}

	return;
}