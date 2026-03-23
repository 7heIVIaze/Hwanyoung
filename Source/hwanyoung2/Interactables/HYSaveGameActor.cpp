// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYSaveGameActor.h"

#include "Player/HYPlayerCharacController.h"
#include "System/HYGameInstance.h"
#include "FMODBlueprintStatics.h"
#include "HwanyoungAssetManager.h"

AHYSaveGameActor::AHYSaveGameActor()
{
	SavepointMesh = CreateDefaultSubobject<UStaticMeshComponent>("SavepointMesh");
	RootComponent = Cast<USceneComponent>(SavepointMesh);

	Super::Name = NSLOCTEXT("HYSaveGameActor", "InteractionName", "Haetae statue");
	Super::Action = NSLOCTEXT("HYSaveGameActor", "InteractionAction", "touch");
}

void AHYSaveGameActor::Interact_Implementation(APlayerController* Controller)
{
	// check if it is in base camp
	if (bIsBaseCampHaeTae)
	{
		// Save the current teleport location to save game for loading save data.
		auto GameInst = GetGameInstance<UHYGameInstance>();

		GameInst->UpdateBaseCampHaeTae(TeleportLocation, FName(*GameInst->TargetLevelName));
	}

	AHYPlayerCharacController* IController = Cast<AHYPlayerCharacController>(Controller);
	IController->ToggleHaetaeStatueHUD(TeleportLocation, Description);

	Super::Interact_Implementation(Controller);

	IController->SaveGameplay();

	FStreamableManager& AssetLoader = UHwanyoungAssetManager::GetStreamableManager();
	AssetLoader.RequestAsyncLoad(SaveEvent.ToSoftObjectPath(), FStreamableDelegate::CreateLambda([this]()
	{
		if (UFMODEvent* LoadedEvent = SaveEvent.Get())
		{
			UFMODBlueprintStatics::PlayEvent2D(GetWorld(), LoadedEvent, true);
		}
	}));

	HaetaeToTeleport->HaetaeToTeleport = this;
	HaetaeToTeleport->TeleportLocation = GetActorLocation()
		+ GetActorForwardVector() * 200.0f;
}

ESaveApplyState AHYSaveGameActor::ApplySaveData(FSaveData SaveData)
{
	// If the save actor is in base camp
	if (bIsBaseCampHaeTae)
	{
		auto GameInst = GetGameInstance<UHYGameInstance>();
		if (FVector *Location = SaveData.SavedBaseCampLocation.Find(FName(*GameInst->TargetLevelName)))
		{
			TeleportLocation = *Location;
			return ESaveApplyState::Changed;
		}

		// There is no save data of teleport location
		return ESaveApplyState::None;
	}

	// It is not in base camp, so there is no applicable save
	return ESaveApplyState::None;
}

void AHYSaveGameActor::BeginPlay()
{
	// Set Description text.
	FText DescriptionText = bIsBaseCampHaeTae
		? NSLOCTEXT("AHYSaveGameActor", "InBasecampDescription", "Teleport to level")
		: NSLOCTEXT("AHYSaveGameActor", "InLevelDescription", "Teleport to the base camp");

	Description = DescriptionText;

	// If HaetaeToTeleport is valid
	if (IsValid(HaetaeToTeleport))
	{
		TeleportLocation = HaetaeToTeleport->GetActorLocation() 
			+ HaetaeToTeleport->GetActorForwardVector() * 200.0f;
	}
	// else
	else
	{
		auto GameInst = GetGameInstance<UHYGameInstance>();
		if (FTransform* Transform = GameInst->SaveData.SavedLevelTransform.Find(FName(*GameInst->TargetLevelName)))
		{
			TeleportLocation = Transform->GetLocation();
		}
	}
}
