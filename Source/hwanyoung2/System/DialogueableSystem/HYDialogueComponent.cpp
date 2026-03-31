// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYDialogueComponent.h"
#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"
#include "DataAssets/DialogueType.h"
#include "Character/HYPlayerCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/HYPlayerCharacController.h"
#include "UI/HYDialogueWidget.h"
#include "HYGameInstance.h"
#include "HYSaveGame.h"
#include "Components/ScrollBox.h"
#include "UI/HYDialogueOptionWidget.h"

// Sets default values for this component's properties
UHYDialogueComponent::UHYDialogueComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UHYDialogueComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UHYDialogueComponent::OptionSelected_Implementation(UDataTable* DialogueTable, FName NextRow)
{
	FDialogueStructure* DialogueData = DialogueTable->FindRow<FDialogueStructure>(NextRow, TEXT(""));

	DialogueWidget->OptionScrollBox->ClearChildren();

	PlaySounds(DialogueData->MelodicSFXOneShot, DialogueData->EffortBarkline);
	DialogueWidget->UpdateDialogue(DialogueData->DialogueText, Name, DialogueData->SpeakerPortrait);

	RowName = DialogueData->NextRow;
	bIsDialogueEnded = DialogueData->bIsDialogueEnded;
	bIsEventTrigger = DialogueData->bIsEventTrigger;
	DataTable = DialogueData->DataTable;
	bIsReplyDialogueOpened = DialogueData->bIsReplyDialogueOpened;
	EventTriggerType = DialogueData->EventTriggerType;

	if (true == bIsEventTrigger)
	{
		OnDialogueEventTrigger.Broadcast(EventTriggerType);
	}

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, DialogueData]()
	{
		if (true == this->bIsReplyDialogueOpened)
		{
			this->ShowReplyOptions(DialogueData->ReplyOptions);
		}
		else
		{
			this->DialogueWidget->ReplyingEnd();
		}
	}), 1.0f, false);
	
}

void UHYDialogueComponent::PlaySounds_Implementation(UFMODEvent* MelodicSeq, UFMODEvent* EffortBankLine)
{
}

void UHYDialogueComponent::StartDialogue(AHYPlayerCharacterBase* PlayerRef)
{
	// Only move to next line when reply options are not shown.
	if (false == bIsReplyDialogueOpened)
	{
		// Should dialogue be ended?
		if (true == bIsDialogueEnded)
		{
			EndDialogue(PlayerRef);
		}
		else
		{
			FDialogueStructure* DialogueData = DataTable->FindRow<FDialogueStructure>(RowName, TEXT(""));
			
			// If the dialogue widget is valid, ignore create a new widget
			if (IsValid(DialogueWidget))
			{
				if (DialogueData)
				{
					PlaySounds(DialogueData->MelodicSFXOneShot, DialogueData->EffortBarkline);
					DialogueWidget->UpdateDialogue(DialogueData->DialogueText, Name, DialogueData->SpeakerPortrait);
				}
			}
			else
			{
				PlayerRef->RemovePlayerHUDWidget();
	
				if (DialogueData)
				{
					DialogueWidget = CreateWidget<UHYDialogueWidget>(DialogueWidgetClass);

					DialogueWidget->Init(DialogueData->DialogueText, Name, DialogueData->SpeakerPortrait, this);
					DialogueWidget->AddToViewport();

					PlaySounds(DialogueData->MelodicSFXOneShot, DialogueData->EffortBarkline);

					PlayerRef->ControllerRef->SetInputMode(FInputModeGameAndUI());
					PlayerRef->ControllerRef->SetShowMouseCursor(true);
				}
			}

			RowName = DialogueData->NextRow;
			DataTable = DialogueData->DataTable;
			bIsDialogueEnded = DialogueData->bIsDialogueEnded;
			bIsEventTrigger = DialogueData->bIsEventTrigger;
			bIsReplyDialogueOpened = DialogueData->bIsReplyDialogueOpened;
			EventTriggerType = DialogueData->EventTriggerType;

			// Show reply options
			if (true == bIsReplyDialogueOpened)
			{
				ShowReplyOptions(DialogueData->ReplyOptions);
			}
		}
	}
}

void UHYDialogueComponent::EndDialogue(AHYPlayerCharacterBase* PlayerRef)
{
	if (IsValid(DialogueWidget))
	{
		UpdateLore();
		DialogueWidget->RemoveFromParent();
		bIsDialogueEnded = false;
		bIsReplyDialogueOpened = false;

		if (true == bIsEventTrigger)
		{
			OnDialogueEventTrigger.Broadcast(EventTriggerType);
		}

		bIsEventTrigger = false;
		DialogueWidget = nullptr;
		PlayerRef->AddPlayerHUDWidgetToViewport();
	}
}

void UHYDialogueComponent::UpdateLore()
{
	UHYGameInstance* GameInstance = Cast<UHYGameInstance>(UGameplayStatics::GetGameInstance(this));
	
	if (nullptr != GameInstance)
	{
		if (GameInstance->SaveData.LoreData.Contains(FName(*Name.ToString())))
		{
			auto UpdatedLoreData = GameInstance->SaveData.LoreData.Find(FName(*Name.ToString()));

			if (false == UpdatedLoreData->bHasInteracted)
			{
				UpdatedLoreData->bHasInteracted = true;
				GameInstance->UpdateLore(FName(*Name.ToString()), *UpdatedLoreData);
				GameInstance->SaveFileToSlot();
			}
		}
		else
		{
			FLore LoreData(FName(*Name.ToString()), true, false, false);
			GameInstance->UpdateLore(FName(*Name.ToString()), LoreData);
			GameInstance->SaveFileToSlot();
		}
	}
}

void UHYDialogueComponent::ShowReplyOptions(TArray<FOptionsDialogue> _ReplyOptions)
{
	UGameplayStatics::SetGamePaused(this, true);

	for (FOptionsDialogue ReplyOption : _ReplyOptions)
	{
		UHYDialogueOptionWidget* ReplyWidget = CreateWidget<UHYDialogueOptionWidget>(DialogueOptionWidgetClass);
		ReplyWidget->Init(this, ReplyOption.OptionText, ReplyOption.RowName, ReplyOption.DataTable);

		// Add options to dialogue widget
		DialogueWidget->OptionScrollBox->AddChild(ReplyWidget);

		// Disable to move next line by pressing F or clicking script box.
		DialogueWidget->ReplyingTime();
	}
}

void UHYDialogueComponent::SkipDialogues()
{
	FText DialogueText;
	UTexture2D* Portrait;
	// skip until reply option(choice) opened or dialogue end
	while (!bIsReplyDialogueOpened && !bIsDialogueEnded)
	{
		FDialogueStructure* DialogueData = DataTable->FindRow<FDialogueStructure>(RowName, TEXT(""));

		RowName = DialogueData->NextRow;
		DataTable = DialogueData->DataTable;
		bIsDialogueEnded = DialogueData->bIsDialogueEnded;
		bIsEventTrigger = DialogueData->bIsEventTrigger;
		bIsReplyDialogueOpened = DialogueData->bIsReplyDialogueOpened;
		EventTriggerType = DialogueData->EventTriggerType;
		DialogueText = DialogueData->DialogueText;
		Portrait = DialogueData->SpeakerPortrait;
		ReplyDialogueOptions = DialogueData->ReplyOptions;
	}

	DialogueWidget->UpdateDialogue(DialogueText, Name, Portrait);
}
