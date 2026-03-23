// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HYInteractableActor.h"
#include "Interfaces/SaveDataReceiver.h"
#include "HYSaveGameActor.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API AHYSaveGameActor : public AHYInteractableActor, public ISaveDataReceiver
{
	GENERATED_BODY()

public: 
	// Sets default values for this actor's properties
	AHYSaveGameActor();

	void Interact_Implementation(APlayerController* Controller) override;

#pragma region Interfaces
public:
	// Apply save data, but it is for getting Teleport Location
	virtual ESaveApplyState ApplySaveData(FSaveData SaveData) override;
#pragma endregion

#pragma region Get/Set
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetTeleportLocation() { return TeleportLocation; }
#pragma endregion

#pragma region Event Graphs
protected:
	virtual void BeginPlay() override;
#pragma endregion

protected: 
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* SavepointMesh;

#pragma region Variables
	// For teleport to base camp or level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport", meta=(AllowPrivateAccess="true"))
	bool bIsBaseCampHaeTae;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Teleport", meta=(AllowPrivateAccess="true"))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Teleport", meta=(AllowPrivateAccess="true"))
	TSoftObjectPtr<class UFMODEvent> SaveEvent;

public:
	// For getting the location of teleport
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport", meta=(AllowPrivateAccess="true"))
	AHYSaveGameActor* HaetaeToTeleport;

	// Set the teleport location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport", meta=(AllowPrivateAccess="true"))
	FVector TeleportLocation;
#pragma endregion
};
