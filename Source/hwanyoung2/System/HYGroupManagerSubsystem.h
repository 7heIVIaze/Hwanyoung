// // Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "HYGroupManagerSubsystem.generated.h"

/**
 * 
 */
class AHYEnemyBase;

UCLASS()
class HWANYOUNG2_API UHYGroupManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void RegisterGroup(AHYEnemyBase* Mobs, FName GroupId);

	UFUNCTION(BlueprintCallable)
	void NotifyGroupAttacked(FName GroupId, AActor* AttackTarget);

	UFUNCTION(BlueprintCallable)
	void RemoveFromGroup(AHYEnemyBase* Mobs, FName GroupId);
	
private:
	TMap<FName, TArray<AHYEnemyBase*>> Group;
};
