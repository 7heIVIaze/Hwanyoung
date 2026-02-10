// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYDialogueWidget.h"
#include "System/DialogueableSystem/HYDialogueComponent.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Character/HYPlayerCharacterBase.h"

void UHYDialogueWidget::NativeConstruct()
{
	FInputModeUIOnly UIInputMode;
	UIInputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);

	// Remove player main HUD and make ui input mode
	AHYPlayerCharacterBase* Player = Cast<AHYPlayerCharacterBase>(GetOwningPlayer()->GetPawn());
	Player->RemovePlayerHUDWidget_Implementation();

	GetOwningPlayer()->bShowMouseCursor = true;

	GetOwningPlayer()->SetInputMode(UIInputMode);
}

void UHYDialogueWidget::NativeDestruct()
{
	// Add player main HUD and make game input mode
	FInputModeGameOnly GameInputMode;
	GetOwningPlayer()->SetInputMode(GameInputMode);
	GetOwningPlayer()->bShowMouseCursor = false;

	AHYPlayerCharacterBase* Player = Cast<AHYPlayerCharacterBase>(GetOwningPlayer()->GetPawn());
	Player->AddPlayerHUDWidgetToViewport_Implementation();
}

void UHYDialogueWidget::UpdateDialogue(FText _inText, FText _SpeakerName, UTexture2D* _SpeakerPortrait)
{
	DialogueTextBlock->SetText(_inText);
	SpeakerText->SetText(_SpeakerName);
	SpeakerTexture->SetBrushFromTexture(_SpeakerPortrait);
}

void UHYDialogueWidget::ReplyingEnd()
{
	SkipButton->SetVisibility(ESlateVisibility::Visible);
	NextButton->SetVisibility(ESlateVisibility::Visible);
}

void UHYDialogueWidget::ReplyingTime()
{
	SkipButton->SetVisibility(ESlateVisibility::Collapsed);
	NextButton->SetVisibility(ESlateVisibility::Collapsed);
}

void UHYDialogueWidget::NextButtonClick()
{
	if (IsValid(DialogueComponent))
	{
		DialogueComponent->StartDialogue(Cast<AHYPlayerCharacterBase>(GetOwningPlayer()->GetPawn()));
	}
}

void UHYDialogueWidget::SkipButtonClick()
{
	if (IsValid(DialogueComponent))
	{
		// Skip until dialogue end or reply option is shown
		while (!(DialogueComponent->bIsDialogueEnded && DialogueComponent->bIsReplyDialogueOpened))
		{
			DialogueComponent->StartDialogue(Cast<AHYPlayerCharacterBase>(GetOwningPlayer()->GetPawn()));
		}
	}
}

void UHYDialogueWidget::Init(FText _DialogueText, FText _SpeakerName, UTexture2D* _SpeakerPortrait, UHYDialogueComponent* _DialogueComp)
{
	
	DialogueComponent = _DialogueComp;

	DialogueTextBlock->SetText(_DialogueText);
	SpeakerText->SetText(_SpeakerName);
	SpeakerTexture->SetBrushFromTexture(_SpeakerPortrait);
}
