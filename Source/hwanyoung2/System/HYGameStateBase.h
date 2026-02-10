// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "Engine.h"
#include "GameFramework/GameStateBase.h"
#include "HYGameStateBase.generated.h"

/**
 * The game state holds the database for all existing items within the game.
 */
UCLASS()
class HWANYOUNG2_API AHYGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public: 
	AHYGameStateBase();
	
	//getter function for item database
	UDataTable* GetItemDatabase() const; 

protected:

	UPROPERTY(EditDefaultsOnly)
		class UDataTable* ItemDatabase;
};
