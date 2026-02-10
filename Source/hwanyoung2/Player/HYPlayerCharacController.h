// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "Interactables/HYInteractableActor.h"
#include "Inventory/InventoryItem.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HYPlayerCharacController.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API AHYPlayerCharacController : public APlayerController
{
	GENERATED_BODY()

public: 
	//setting up the controller inventory
	AHYPlayerCharacController();

protected:
	virtual void BeginPlay() override;

public:
#pragma region inventory
	
	UFUNCTION(BlueprintImplementableEvent)
	void ReloadInventory();

	UFUNCTION(BlueprintImplementableEvent)
	void ReloadCraftUI();

	UFUNCTION(BlueprintImplementableEvent)
	void SaveGameplay();

	// Lose Honbaek coz of death.
	UFUNCTION(BlueprintNativeEvent)
	void LoseHonbaek();

	virtual void LoseHonbaek_Implementation();

	// Use honbaek as currency
	UFUNCTION(BlueprintNativeEvent)
	void UseHonbaek(int Amount);

	virtual void UseHonbaek_Implementation(int Amount);

	// Collect honbaek as currency
	UFUNCTION(BlueprintNativeEvent)
	void CollectHonbaek(int Amount);

	virtual void CollectHonbaek_Implementation(int Amount);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	int32 GetInventoryWeight();

	UFUNCTION(BlueprintCallable, Category = "Utils")
	bool AddItemToInventoryByID(FName ID);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	FInventoryItem CraftItem(FInventoryItem ItemA);

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class AHYInteractableActor* CurrentInteractable;

	// Inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FInventoryItem> Inventory;

	// Equipped buff items
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FInventoryItem> BuffItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InventorySlotLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InventoryWeightLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Currency;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TSet<FName> CraftingItems;

	//interacts with the Interactable objects
	UFUNCTION(BlueprintCallable, Category = "Utils")
	void Interact();

#pragma endregion

#pragma region UI
	// Inventory Widget object(instance)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI/UX")
	TObjectPtr<class UUserWidget> InventoryWidget;

	// Inventory widget class
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI/UX")
	TSubclassOf<class UUserWidget> InventoryHUDWidget;

	// Crafting Widget object(instance)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI/UX")
	TObjectPtr<class UUserWidget> CraftingWidget;

	// Crafting widget class
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI/UX")
	TSubclassOf<class UUserWidget> CraftingHUDWidget;

	// Cooking Widget object(instance)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI/UX")
	TObjectPtr<class UUserWidget> CookingWidget;

	// Cooking widget class
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI/UX")
	TSubclassOf<class UUserWidget> CookingHUDWidget;

	// Tutorial Widget object(instance)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI/UX")
	TObjectPtr<class UUserWidget> TutorialWidget;

	// Tutorial widget class
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI/UX")
	TSubclassOf<class UUserWidget> TutorialHUDWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI/UX")
	TObjectPtr<class UUserWidget> HaeTaeWidget;

	// HaeTae Statue Widget class
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI/UX")
	TSubclassOf<class UUserWidget> HaeTaeHUDWidget;

	UFUNCTION(BlueprintCallable)
	virtual void SetupHUDReference();

	// Toggling Inventory HUD
	UFUNCTION(BlueprintNativeEvent)
	void ToggleInventoryWidget();

	virtual void ToggleInventoryWidget_Implementation();

	// Toggling Crafting HUD
	UFUNCTION(BlueprintNativeEvent)
	void ToggleCraftingWidget();

	virtual void ToggleCraftingWidget_Implementation();

	// Toggling Cooking HUD
	UFUNCTION(BlueprintNativeEvent)
	void ToggleCookingWidget();

	virtual void ToggleCookingWidget_Implementation();

	// Show Tutorial HUD
	UFUNCTION(BlueprintNativeEvent)
	void ShowTutorialWidget(FName TutorialName, bool AddedOnAnotherHUD);

	virtual void ShowTutorialWidget_Implementation(FName TutorialName, bool AddedOnAnotherHUD);

#pragma endregion


#pragma region SaveData
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SaveData")
	FString SaveFilename;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
	TObjectPtr<class UHYSaveGame> SaveObject;

	UFUNCTION(BlueprintCallable)
	virtual bool SaveGame();

	UFUNCTION(BlueprintCallable)
	virtual bool LoadGameSaveFile();

	UFUNCTION(BlueprintNativeEvent)
	void LoadSaveFileOrInitialize();

	virtual void LoadSaveFileOrInitialize_Implementation();

	// Toggle the haetae HUD when interacting with the haetae statue
	UFUNCTION(BlueprintCallable)
	void ToggleHaetaeStatueHUD(FVector TeleportLocation, FText DescriptionText, bool IsTeleportHaeTae);
#pragma endregion


#pragma region Dialogue
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Diaglogue System")
	UDataTable* DT_TutorialText;

	UFUNCTION(BlueprintNativeEvent)
	void StartDialogue();

	virtual void StartDialogue_Implementation();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Diaglogue System")
	AActor* CurrentDialogueNPC;
#pragma endregion


protected:

	//abstract function to override in order to 
	//bind actions or axis to different input components
	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn) override;
};
