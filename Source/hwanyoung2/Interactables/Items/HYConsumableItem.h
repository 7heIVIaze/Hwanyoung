// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HYManualPickUp.h"
#include "HYConsumableItem.generated.h"

/**
 * 
 */


UCLASS()
class HWANYOUNG2_API AHYConsumableItem : public AHYManualPickUp
{
	GENERATED_BODY()

public: 
	AHYConsumableItem();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Pick up and consume")
	void OnUsed(APlayerController* Controller);
	
};
