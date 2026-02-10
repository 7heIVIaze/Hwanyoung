// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HYAutomaticPickUp.h"
#include "HYMoneyAutoPickUp.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API AHYMoneyAutoPickUp : public AHYAutomaticPickUp
{
	GENERATED_BODY()

public:
	AHYMoneyAutoPickUp();

	void Collect_Implementation(APlayerController* Controller) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value;
	
};
