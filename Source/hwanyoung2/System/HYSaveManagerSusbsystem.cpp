// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYSaveManagerSusbsystem.h"

#include "HYSaveGame.h"

void UHYSaveManagerSusbsystem::SavePuzzleCompleted(FName PuzzleId)
{
	SaveGame->UpdateClearedPuzzleData(PuzzleId);
}
