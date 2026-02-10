// Copyright 2024. TEAM DAON. All rights reserved.


#include "HwanyoungAssetManager.h"
#include "AbilitySystemGlobals.h"

void UHwanyoungAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	UAbilitySystemGlobals::Get().InitGlobalData();
	UE_LOG(LogTemp, Warning, TEXT("Hello World!!"));
}
