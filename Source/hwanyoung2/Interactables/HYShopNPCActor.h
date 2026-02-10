// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HYInteractableActor.h"
#include "Inventory/InventoryItem.h"
#include "HYShopNPCActor.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API AHYShopNPCActor : public AHYInteractableActor
{
	GENERATED_BODY()

public:
	virtual void Interact_Implementation(APlayerController* Controller);

	// This Function will be called when player buy the item.
	// @param Controller: Player Controller, ItemToBuy: the item structure(Data) to buy, Amount: the number of the item.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BuyTradableItem(APlayerController* Controller, const FInventoryItem ItemToBuy, const int Amount);

	// This Function will be called when player buy the item.
	// @param Controller: Player Controller, ItemToBuy: the item structure(Data) to buy, Amount: the number of the item.
	virtual void BuyTradableItem_Implementation(APlayerController* Controller, const FInventoryItem ItemToBuy, const int Amount);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<FInventoryItem, int> TradableItems;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> ShopWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widget")
	TObjectPtr<class UUserWidget> ShopWidget;*/
};
