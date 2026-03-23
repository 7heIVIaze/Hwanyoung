// // Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "HYWorldSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API UHYWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

#pragma region Teammates
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
#pragma endregion

};
