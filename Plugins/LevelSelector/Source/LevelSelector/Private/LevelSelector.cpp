// Copyright Epic Games, Inc. All Rights Reserved.

#include "LevelSelector.h"
#include "LevelSelectorStyle.h"
#include "LevelSelectorCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "FileHelpers.h"
#include "LevelEditor.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"

static const FName LevelSelectorTabName("LevelSelector");

#define LOCTEXT_NAMESPACE "FLevelSelectorModule"

void FLevelSelectorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FLevelSelectorStyle::Initialize();
	FLevelSelectorStyle::ReloadTextures();

	FLevelSelectorCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	/*PluginCommands->MapAction(
		FLevelSelectorCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FLevelSelectorModule::PluginButtonClicked),
		FCanExecuteAction());*/

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FLevelSelectorModule::RegisterMenus));
}

void FLevelSelectorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FLevelSelectorStyle::Shutdown();

	FLevelSelectorCommands::Unregister();
}

TSharedRef<SWidget> FLevelSelectorModule::GenerateDropdownMenuBar()
{
	TArray<FString> LevelNames;
	const FString LevelsPath = FPaths::ProjectContentDir() + TEXT("Hwanyoung/Levels/");
	IFileManager::Get().FindFilesRecursive(LevelNames, *LevelsPath, TEXT("*.umap"), true, false);

	FMenuBuilder MenuBuilder(true, nullptr);

	for (const FString& LevelPath : LevelNames)
	{
		FString NameOnly = FPaths::GetBaseFilename(LevelPath);
		MenuBuilder.AddMenuEntry(
			FText::FromString(*NameOnly),
			FText::FromString(*NameOnly),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateRaw(this, &FLevelSelectorModule::OnLevelSelected, NameOnly))
		);
	}

	return MenuBuilder.MakeWidget();
}

void FLevelSelectorModule::OnLevelSelected(FString LevelName)
{
	SelectedLevelName = LevelName;
	UE_LOG(LogTemp, Log, TEXT("Selected Level: %s"), *LevelName);
}

void FLevelSelectorModule::PlayButtonClicked()
{
	if(SelectedLevelName.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("No level selected to play."));
		return;
	}

	const FString LevelToPlay = FString::Printf(TEXT("/Game/Hwanyoung/Levels/%s"), *SelectedLevelName);
	if(!FPackageName::DoesPackageExist(LevelToPlay))
	{
		UE_LOG(LogTemp, Warning, TEXT("Level '%s' does not exist."), *LevelToPlay);
		return;
	}

	UEditorLoadingAndSavingUtils::LoadMap(LevelToPlay);

	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
	TSharedPtr<IAssetViewport> ActiveLevelViewport = LevelEditorModule.GetFirstActiveViewport();

	if (GUnrealEd)
	{
		FRequestPlaySessionParams SessionParams;
		SessionParams.WorldType = EPlaySessionWorldType::PlayInEditor;
		SessionParams.SessionDestination = EPlaySessionDestinationType::InProcess;
		SessionParams.DestinationSlateViewport = ActiveLevelViewport;
		GUnrealEd->RequestPlaySession(SessionParams);
	}

	// Put your "OnButtonClicked" stuff here
	/*FText DialogText = FText::Format(
							LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
							FText::FromString(TEXT("FLevelSelectorModule::PluginButtonClicked()")),
							FText::FromString(TEXT("LevelSelector.cpp"))
					   );
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);*/
}

void FLevelSelectorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	{
		FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
		FToolMenuEntry DropdownEntry = FToolMenuEntry::InitComboButton(
			"LevelSelectorDropdown",
			FUIAction(),
			FOnGetContent::CreateRaw(this, &FLevelSelectorModule::GenerateDropdownMenuBar),
			TAttribute<FText>::Create([this]() {
			return SelectedLevelName.IsEmpty()
				? FText::FromString("Level Selector")
				: FText::FromString(FString::Printf(TEXT("[ %s ] "), *SelectedLevelName));
			}),
			LOCTEXT("LevelSelectorDropdown_Tooltip", "Select a level to play"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Level")
		);
		DropdownEntry.StyleNameOverride = "CalloutToolbar";
		Section.AddEntry(DropdownEntry);

		FToolMenuEntry PlayButtonEntry = FToolMenuEntry::InitToolBarButton(
			"PlaySelectedLevel",
			FUIAction(FExecuteAction::CreateRaw(this, &FLevelSelectorModule::PlayButtonClicked)),
			LOCTEXT("PlayLevel_Label", "Play Level"),
			LOCTEXT("PlayLevel_Tooltip", "Play selected level"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Play")
		);

		Section.AddEntry(PlayButtonEntry);

		//{
		//	FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FLevelSelectorCommands::Get().PluginAction));
		//	Entry.SetCommandList(PluginCommands);
		//}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLevelSelectorModule, LevelSelector)