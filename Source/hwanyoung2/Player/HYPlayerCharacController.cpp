// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYPlayerCharacController.h"
#include "HYGameStateBase.h"
#include "Engine/DataTable.h"
#include "Character/hwanyoung2Character.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "HYSaveGame.h"
#include "HYGameInstance.h"
#include "Character/HYPlayerCharacterBase.h"
#include "Interfaces/Dialogueable.h"
#include "DialogueableSystem/HYDialogueComponent.h"

AHYPlayerCharacController::AHYPlayerCharacController()
{
	//this is hardcoding the limits:
	InventorySlotLimit = 50; 
	InventoryWeightLimit = 500;
}

void AHYPlayerCharacController::BeginPlay()
{
	Super::BeginPlay();

	// Set up HUD references
	SetupHUDReference();

	AHYPlayerCharacterBase* Character = Cast<AHYPlayerCharacterBase>(GetPawn());
	
	FText CharacterName = Character->GetCharacterName();
	Cast<UHYGameInstance>(GetGameInstance())->GetCharacterSaveData(*CharacterName.ToString());

	// If successfully loaded the save file, update honbaek currency for HUD
	if (LoadGameSaveFile())
	{
		Character->UpdateHonbaek(Currency);
	}
}

void AHYPlayerCharacController::LoseHonbaek_Implementation()
{
	// When died, player loses 3 honbaek, clamped to a minimum of 0
	Currency = FMath::Clamp(Currency - 3, 0, 10000000);
}

void AHYPlayerCharacController::UseHonbaek_Implementation(int Amount)
{
	Currency = FMath::Clamp(Currency - Amount, 0, 10000000);
	Cast<AHYPlayerCharacterBase>(GetPawn())->UpdateHonbaek(Currency);
}

void AHYPlayerCharacController::CollectHonbaek_Implementation(int Amount)
{
	Currency = FMath::Clamp(Currency + Amount, 0, 10000000);
	Cast<AHYPlayerCharacterBase>(GetPawn())->UpdateHonbaek(Currency);
}

int32 AHYPlayerCharacController::GetInventoryWeight()
{
	int32 TotalWeight = 0;

	for (auto& Item : Inventory) {
		TotalWeight += 1; //hard coding for now
	}
	return TotalWeight;
}

bool AHYPlayerCharacController::AddItemToInventoryByID(FName ID)
{
	AHYGameStateBase* gameState = Cast<AHYGameStateBase>(GetWorld()->GetGameState());
	UDataTable* itemDB = gameState->GetItemDatabase();
	FInventoryItem* itemToAdd = itemDB->FindRow<FInventoryItem>(ID, "");

	if (itemToAdd) {
		if (Inventory.Num() < InventorySlotLimit &&
			GetInventoryWeight() + itemToAdd->ItemWeight <= InventoryWeightLimit) {
			Inventory.Add(*itemToAdd);
			ReloadInventory();
			return true;
		}
	}
	return false;
}

#pragma region By Teammate
FInventoryItem AHYPlayerCharacController::CraftItem(FInventoryItem ItemA)
{
	/*for (auto Combination : ItemB.CraftCombinations) {
		if (Combination.ItemAID == ItemA.ItemID 
			&& Combination.ItemBID == ItemB.ItemID
			&& Combination.ItemCID == ItemC.ItemID 
			&& Combination.ItemDID == ItemD.ItemID) {
			if (Combination.bDestroyItemA) Inventory.RemoveSingle(ItemA);
			if (Combination.bDestroyItemB) Inventory.RemoveSingle(ItemB);
			if (Combination.bDestroyItemC) Inventory.RemoveSingle(ItemC);
			if (Combination.bDestroyItemD) Inventory.RemoveSingle(ItemD);
			AddItemToInventoryByID(Combination.ProductID);
			ReloadCraftUI();
		}
	}*/

	int32 NumOfCraftingItems = CraftingItems.Num();
	bool bIsValidComb;
	bIsValidComb = false;
	if (NumOfCraftingItems < 2 || NumOfCraftingItems > 4) { return FInventoryItem(); }

	AHYGameStateBase* gameState = Cast<AHYGameStateBase>(GetWorld()->GetGameState());
	UDataTable* itemDB = gameState->GetItemDatabase();
	
	for (auto Combination : ItemA.CraftCombinations) {
		if (CraftingItems.Contains(Combination.ItemBID) && NumOfCraftingItems >= 2) {
			if (Combination.bDestroyItemB) Inventory.RemoveSingle(*itemDB->FindRow<FInventoryItem>(Combination.ItemBID, ""));
			bIsValidComb = bIsValidComb || true;
		}
		if (CraftingItems.Contains(Combination.ItemCID) && NumOfCraftingItems >= 3) {
			if (Combination.bDestroyItemC) Inventory.RemoveSingle(*itemDB->FindRow<FInventoryItem>(Combination.ItemCID, ""));
			bIsValidComb = bIsValidComb && true;
		}
		if (CraftingItems.Contains(Combination.ItemDID) && NumOfCraftingItems == 4) {
			if (Combination.bDestroyItemD) Inventory.RemoveSingle(*itemDB->FindRow<FInventoryItem>(Combination.ItemDID, ""));
			bIsValidComb = bIsValidComb && true;
		}
		if (bIsValidComb) {
			if (Combination.bDestroyItemA) Inventory.RemoveSingle(ItemA);
			AddItemToInventoryByID(Combination.ProductID);
			ReloadCraftUI();
			return *itemDB->FindRow<FInventoryItem>(Combination.ProductID, "");
		}
		else continue;
	}
	return FInventoryItem();
}

void AHYPlayerCharacController::Interact()
{
	if (CurrentInteractable) CurrentInteractable->Interact(this);
}

void AHYPlayerCharacController::SetupHUDReference()
{
	if (nullptr != InventoryHUDWidget)
	{
		InventoryWidget = CreateWidget<UUserWidget>(this, InventoryHUDWidget);
	}

	if (nullptr != CraftingHUDWidget)
	{
		CraftingWidget = CreateWidget<UUserWidget>(this, CraftingHUDWidget);
	}

	if (nullptr != CookingHUDWidget)
	{
		CookingWidget = CreateWidget<UUserWidget>(this, CookingHUDWidget);
	}
}

void AHYPlayerCharacController::ToggleInventoryWidget_Implementation()
{
	// Exceptional case
	if (nullptr == InventoryWidget)
	{
		InventoryWidget = CreateWidget(this, InventoryHUDWidget);
	}

	// If the widget is in viewport, remove it.
	if (InventoryWidget->IsVisible())
	{
		InventoryWidget->RemoveFromParent();
	}
	// else, add to viewport
	else
	{
		InventoryWidget->AddToViewport();
	}

	ShowTutorialWidget(FName("HowToUseInventory"), true);
}

void AHYPlayerCharacController::ToggleCraftingWidget_Implementation()
{
	// Exceptional case
	if (nullptr == CraftingWidget)
	{
		CraftingWidget = CreateWidget(this, CraftingHUDWidget);
	}

	// If the widget is in viewport, remove it.
	if (CraftingWidget->IsVisible())
	{
		CraftingWidget->RemoveFromParent();
	}
	// else, add to viewport
	else
	{
		CraftingWidget->AddToViewport();
	}
}

void AHYPlayerCharacController::ToggleCookingWidget_Implementation()
{
	// Exceptional case
	if (nullptr == CookingWidget)
	{
		CookingWidget = CreateWidget(this, CookingHUDWidget);
	}

	// If the widget is in viewport, remove it.
	if (CookingWidget->IsVisible())
	{
		CookingWidget->RemoveFromParent();
	}
	// else, add to viewport
	else
	{
		CookingWidget->AddToViewport();
	}
}
#pragma endregion

void AHYPlayerCharacController::ShowTutorialWidget_Implementation(FName _TutorialName, bool _AddedOnAnotherHUD)
{
	if (UHYGameInstance* GameInstance = Cast<UHYGameInstance>(GetGameInstance()))
	{
		if (GameInstance->GetCheckTutorial()[_TutorialName])
		{
			return;
		}

		// Create tutorial widget and add to viewport
		TutorialWidget = CreateWidget(GetWorld(), TutorialHUDWidget);

		TutorialWidget->SetOwningPlayer(this);
		TutorialWidget->InitializeWidget(_TutorialName, DT_TutorialText, _AddedOnAnotherHUD);
		TutorialWidget->AddToViewport();

		// Save this tutorial has been read.
		GameInstance->UpdateTutorialChecked(_TutorialName);
	}
}

bool AHYPlayerCharacController::SaveGame()
{
	UHYGameInstance* GameInstance = Cast<UHYGameInstance>(GetGameInstance());

	if(nullptr == GameInstance)
	{
		return false;
	}

	// If the target level name is empty, then update the player transform from the current level name
	if (GameInstance->TargetLevelName == TEXT(""))
	{
		FString LevelName = UGameplayStatics::GetCurrentLevelName(this);

		GameInstance->UpdatePlayerTransformInLevel(*LevelName, GetPawn()->GetActorTransform());
	}
	// else, use the target level name as the key to save the player transform
	else
	{
		GameInstance->UpdatePlayerTransformInLevel(*GameInstance->TargetLevelName, GetPawn()->GetActorTransform());
	}

	// Update currency and inventory, equipped buff items
	GameInstance->UpdateHonbaekAmount(Currency);
	GameInstance->UpdateInventoryItems(Inventory);
	GameInstance->UpdateBuffItemList(BuffItems);

	return GameInstance->SaveFileToSlot();

#pragma region deprecated
	//// Check the save file exists, bc this function could be called by another classes.
	//if (UGameplayStatics::DoesSaveGameExist(SaveFilename, 0))
	//{
	//	if (nullptr == SaveObject)
	//	{
	//		SaveObject = Cast<UHYSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveFilename, 0));
	//	}
	//}
	//else
	//{
	//	SaveObject = Cast<UHYSaveGame>(UGameplayStatics::CreateSaveGameObject(UHYSaveGame::StaticClass()));
	//}
	//AHYPlayerCharacterBase* PC = Cast<AHYPlayerCharacterBase>(GetPawn());
	//FName PCName = FName(*PC->GetCharacterName().ToString());

	//SaveObject->SetPlayerTransformInLevel(PCName, 
	//	FName(*UGameplayStatics::GetCurrentLevelName(GetWorld())), PC->GetActorTransform());
	//SaveObject->SetCurrency(PCName, Currency);
	//SaveObject->SetInventoryItem(PCName, Inventory);
	//SaveObject->SaveGameToSlot(SaveFilename, 0);
	//
	//if (UHYGameInstance* GameInstance = Cast<UHYGameInstance>(GetGameInstance()))
	//{
	//	SaveObject->SetLoreData(PCName, GameInstance->GetLoreData());
	//	SaveObject->SetCheckTutorial(PCName, GameInstance->GetCheckTutorial());
	//}

	//UGameplayStatics::SaveGameToSlot(SaveObject, SaveFilename, 0);

	//return false;
#pragma endregion
}

bool AHYPlayerCharacController::LoadGameSaveFile()
{
	// Get GameInstance
	UHYGameInstance* GameInstance = Cast<UHYGameInstance>(GetGameInstance());

	if (nullptr == GameInstance)
	{
		return false;
	}
	
	FSaveData& SaveData = GameInstance->SaveData;

	// if there is saved data for the target level
	if (SaveData.SavedLevelTransform.Contains(*GameInstance->TargetLevelName))
	{
		// Set the player transform to the saved location
		GetPawn()->SetActorTransform(SaveData.SavedLevelTransform[*GameInstance->TargetLevelName]);
	}
	
	Currency = SaveData.HonbaekAmount;
	Inventory = SaveData.SavedInventoryItem;

#pragma region deprecated
	//// Check the save file exists, bc this function could be called by another classes.
	//SaveObject = Cast<UHYSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveFilename, 0));

	//FName sCurrentLevelName = FName(*UGameplayStatics::GetCurrentLevelName(GetWorld()));
	//FName PCName = FName(*Cast<AHYPlayerCharacterBase>(GetPawn())->GetCharacterName().ToString());

	//// Change the location of character when the current level is already saved.
	//if (SaveObject->GetSavedLevelAndTransform(PCName).Contains(sCurrentLevelName))
	//{
	//	GetPawn()->SetActorTransform(SaveObject->GetSavedLevelAndTransform(PCName)[sCurrentLevelName]);
	//}
	//// When there is in another level or is not saved, save the current location and level name
	//else
	//{
	//	SaveObject->SetPlayerTransformInLevel(PCName, sCurrentLevelName, GetPawn()->GetActorTransform());
	//}

	//// Update current Honbaek amount and inventory to the saved state.
	//Currency = SaveObject->GetSavedCurrency(PCName);
	//Inventory = SaveObject->GetSavedInventoryItems(PCName);
#pragma endregion

	return true;
}

void AHYPlayerCharacController::ToggleHaetaeStatueHUD(FVector TeleportLocation, FText DescriptionText, bool IsTeleportHaeTae)
{
	if(IsValid(HaeTaeWidget))
	{
		HaeTaeWidget->RemoveFromParent();
	}
	else
	{
		HaeTaeWidget = CreateWidget<UUserWidget>(this, HaeTaeHUDWidget);
		HaeTaeWidget->SetOwningPlayer(this);
	}
	
	// update variables
	HaeTaeWidget->TeleportLocation = TeleportLocation;
	HaeTaeWidget->IsTeleportHaeTae = IsTeleportHaeTae;
	HaeTaeWidget->DescriptionText = DescriptionText;
	
	// Remove Player HUD and add HaeTae HUD
	Cast<AHYPlayerCharacterBase>(GetPawn())->RemovePlayerHUDWidget();
	HaeTaeWidget->AddToViewport();
}

void AHYPlayerCharacController::LoadSaveFileOrInitialize_Implementation()
{
	// Does the save file(data) exist
	if (UGameplayStatics::DoesSaveGameExist(SaveFilename, 0))
	{
		if (LoadGameSaveFile())
		{
			// Load Success
		}
		else
		{
			// Load Failed.
		}
	}
	// the save file does not exist
	else
	{
		SaveGame();
	}
}

void AHYPlayerCharacController::StartDialogue_Implementation()
{
	if (IsValid(CurrentDialogueNPC))
	{
		auto DialogueComponent = CurrentDialogueNPC->GetComponentsByInterface(UDialogueable::StaticClass())[0];

		Cast<UHYDialogueComponent>(DialogueComponent)->StartDialogue(Cast<AHYPlayerCharacterBase>(GetPawn()));
	}
}

void AHYPlayerCharacController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void AHYPlayerCharacController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}
