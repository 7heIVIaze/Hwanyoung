// Copyright Epic Games, Inc. All Rights Reserved.

#include "LevelSelectorCommands.h"

#define LOCTEXT_NAMESPACE "FLevelSelectorModule"

void FLevelSelectorCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "LevelSelector", "Execute LevelSelector action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
