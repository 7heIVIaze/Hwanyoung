// Fill out your copyright notice in the Description page of Project Settings.

#include "HYUIManager.h"
#include "Blueprint/UserWidget.h"

UHYUIManager::UHYUIManager()
{

}

void UHYUIManager::PushUI(UUserWidget* Widget)
{
	if (UIStack.Num() > 0)
	{
		UIStack.Top()->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	
	UIStack.Add(Widget);

	UIStack.Top()->SetFocus();
}

void UHYUIManager::PopUI(class UUserWidget* Widget)
{
	if (UIStack.IsEmpty()) return;

	if (UIStack.Top() == Widget)
	{
		UIStack.Pop();
	}
	else
	{
		UIStack.Remove(Widget);
	}

	if (UIStack.Num() > 0)
	{
		UIStack.Top()->SetFocus();
		UIStack.Top()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

UUserWidget* UHYUIManager::Top()
{
	return UIStack.Top();
}