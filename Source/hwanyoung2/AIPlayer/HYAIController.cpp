// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYAIController.h"

AHYAIController::AHYAIController()
{
	//if we create a new AI instance, it will want to have its own player state:
	bWantsPlayerState = true;
}

void AHYAIController::NoticeEnemy(AActor* InAttackTarget)
{
	// Set attack target and change the current state as attacking
	HandleSensedDamage(InAttackTarget);
}