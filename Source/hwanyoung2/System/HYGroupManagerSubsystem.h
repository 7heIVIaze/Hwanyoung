// // Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "HYGroupManagerSubsystem.generated.h"

/**
 * 
 */
class AHYAggroNPCBase;

UCLASS()
class HWANYOUNG2_API UHYGroupManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void RegisterGroup(AHYAggroNPCBase* Mobs, FName GroupId);

	UFUNCTION(BlueprintCallable)
	void NotifyGroupAttacked(FName GroupId, AActor* AttackTarget);
	
private:
	TMap<FName, TArray<AHYAggroNPCBase*>> Group;
};
