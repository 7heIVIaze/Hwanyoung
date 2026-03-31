// Copyright 2024. TEAM DAON. All rights reserved.

#include "UI/HYDialogueOptionWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "System/DialogueableSystem/HYDialogueComponent.h"

void UHYDialogueOptionWidget::ReplyButtonClicked()
{
	DialogueComp->OptionSelected(DialogueTable, RowName);
}

void UHYDialogueOptionWidget::Init(UHYDialogueComponent* DialogueComponent, FText Reply, FName NextRow, UDataTable* DataTable)
{
	DialogueComp = DialogueComponent;
	ReplyText = Reply;
	RowName = NextRow;
	DialogueTable = DataTable;
}

void UHYDialogueOptionWidget::NativeConstruct()
{
	ReplyTextBlock->SetText(ReplyText);

	ReplyButton->OnClicked.AddDynamic(this, &UHYDialogueOptionWidget::ReplyButtonClicked);
}