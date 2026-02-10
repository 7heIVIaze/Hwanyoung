// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inventory/InventoryItem.h"
#include "HYShopAOE.generated.h"

UCLASS()
class HWANYOUNG2_API AHYShopAOE : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHYShopAOE();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called When Begin Overlapped Event.
	UFUNCTION(BlueprintCallable)
	virtual bool RandomizeTradableItemSets(int CurrentHonbaekAmount);

	// @param Item is NPC Actor's Array.
	// Make random amount of the tradable items set.
	UFUNCTION(BlueprintCallable)
	TMap<FInventoryItem, int> GetTradableItemSets(int CurrentHonbaekAmount);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	TArray<FInventoryItem> FinalItems;
};