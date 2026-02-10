// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "LevelSelectorStyle.h"

class FLevelSelectorCommands : public TCommands<FLevelSelectorCommands>
{
public:

	FLevelSelectorCommands()
		: TCommands<FLevelSelectorCommands>(TEXT("LevelSelector"), NSLOCTEXT("Contexts", "LevelSelector", "LevelSelector Plugin"), NAME_None, FLevelSelectorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
