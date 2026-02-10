// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FLevelSelectorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	TSharedRef<SWidget> GenerateDropdownMenuBar();

	void OnLevelSelected(FString LevelName);

	/** This function will be bound to Command. */
	void PlayButtonClicked();
	
public:
	FString SelectedLevelName;

private:

	void RegisterMenus();


private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
