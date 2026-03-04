// // Copyright 2024. TEAM DAON. All rights reserved.


#include "HYWorldSubsystem.h"
#include "HYGameStateBase.h"
#include "HYInteractableActor.h"
#include "HYTreasureChest.h"
#include "HYTreasureChestActor.h"
#include "HYWorldSettings.h"
#include "HYGameInstance.h"

void UHYWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	UE_LOG(LogTemp, Warning, TEXT("WS: OnWorldBeginPlay called"));
	// Casting the world setting
	const AHYWorldSettings* WS =
		Cast<AHYWorldSettings>(InWorld.GetWorldSettings());

	//if the casted world subsystem is null, return
	if (!WS) 
	{
		UE_LOG(LogTemp, Warning, TEXT("WS: cast failed"));
		return;
	}

#pragma region What I Changed
	const UHYGameInstance* GameInstance = Cast<UHYGameInstance>(InWorld.GetGameInstance());
	ELevelID ContentID = WS->LevelIndex;

	if (GameInstance->LastTargetLevel == TEXT("Sprink_TEST"))
	{
		ContentID = ELevelID::Spring;
	}
#pragma region endregion

	UE_LOG(LogTemp, Warning, TEXT("WS: cast successful"));

	// Sets the pointers for the current game state and chestDT:
	AHYGameStateBase* gameState = Cast<AHYGameStateBase>(InWorld.GetGameState());
	if (!gameState) return;

	UDataTable* chestDT = gameState->GetTreasureChestDT();
	UE_LOG(LogTemp, Warning, TEXT("WS: Chest DT loaded successfully"));

	TArray<FTreasureChest*> DTrows;
	chestDT->GetAllRows<FTreasureChest>(TEXT("GetAllRows"), DTrows);

	auto filtered = DTrows.FilterByPredicate([&ContentID](const FTreasureChest* data) {
		return data->LevelIndex == ContentID;
	});

	/*static ConstructorHelpers::FClassFinder<AHYInteractableActor>
		BP_TreasureChest(TEXT("/Game/Hwanyoung/Interactables/BP_TreasureChest.BP_TreasureChest_C"));*/
	
	UClass* ChestClass = LoadClass<AActor>(nullptr, TEXT("/Game/Hwanyoung/Interactables/BP_TreasureChest.BP_TreasureChest_C"));


	for (const FTreasureChest* data : filtered) {
		FTransform dataTransform(
			FRotator(data->RotationVector.X, data->RotationVector.Z, data->RotationVector.Y),    // Default rotation (no rotation)
			data->WorldLocation,            // The FVector for location
			FVector::OneVector     // Default scale (unit scale)
		);
		AHYTreasureChestActor* spawned = InWorld.SpawnActor<AHYTreasureChestActor>(
			ChestClass,
			dataTransform
		);
		spawned->Initialize(data->ItemIDList);

		FString ItemLists, Lists2;

		for (auto ID : spawned->ItemIDList)
		{
			ItemLists += ID.ToString() + TEXT(", ");
		}

		for (auto ID : data->ItemIDList)
		{	
			Lists2 += ID.ToString() + TEXT(", ");
		}


		UE_LOG(LogTemp, Warning, TEXT("WS: Spawned chest at location %s has %s. It shoud have these %s"), *data->WorldLocation.ToString(), *ItemLists, *Lists2);
	}
}

