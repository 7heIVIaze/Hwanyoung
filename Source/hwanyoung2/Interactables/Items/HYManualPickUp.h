// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/HYInteractableActor.h"
#include "HYManualPickUp.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API AHYManualPickUp : public AHYInteractableActor
{
	GENERATED_BODY()
public:
	AHYManualPickUp();

	void Interact_Implementation(APlayerController* Controller) override;

protected:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PickUpRangeMesh;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName ItemID;
};
