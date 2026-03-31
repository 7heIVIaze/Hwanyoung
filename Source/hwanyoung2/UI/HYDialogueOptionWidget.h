// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HYDialogueOptionWidget.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API UHYDialogueOptionWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Events Graph
public:
	UFUNCTION(BlueprintCallable)
	void ReplyButtonClicked();

	// Initialize this widget
	UFUNCTION(BlueprintCallable)
	void Init(class UHYDialogueComponent* DialogueComponent, FText Reply, FName NextRow, UDataTable* DataTable);

	virtual void NativeConstruct() override;
#pragma endregion
	
#pragma region Components
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta =(BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UButton> ReplyButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta =(BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UTextBlock> ReplyTextBlock;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta =(BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<class UHYDialogueComponent> DialogueComp;

#pragma endregion

#pragma region Variables
	// Reply
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ExposeOnSpawn = true))
	FText ReplyText;

	// Row in the data table
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ExposeOnSpawn = true))
	FName RowName;

	// data table
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ExposeOnSpawn = true))
	UDataTable* DialogueTable;
#pragma endregion
};
