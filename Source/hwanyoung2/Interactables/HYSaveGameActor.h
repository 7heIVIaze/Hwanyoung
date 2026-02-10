// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HYInteractableActor.h"
#include "HYSaveGameActor.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API AHYSaveGameActor : public AHYInteractableActor
{
	GENERATED_BODY()

public: 
	// Sets default values for this actor's properties
	AHYSaveGameActor();

	void Interact_Implementation(APlayerController* Controller) override;

protected: 
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* SavepointMesh;

};
