// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "HYPoolSubSystem.generated.h"

/**
 * 
 */
USTRUCT()
struct FPoolArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<AActor*> ObjectPool;

	bool IsEmpty() { return ObjectPool.IsEmpty(); }

	AActor* Pop() { return ObjectPool.Pop(); }

	void Add(AActor* ActorToAdd) { ObjectPool.Add(ActorToAdd); }
};

UCLASS()
class HWANYOUNG2_API UHYPoolSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Pool Subsystem")
	void SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation, AActor*& SpawnedActor);
	
	UFUNCTION(BlueprintCallable, Category = "Pool Subsystem")
	void ReturnToPool(AActor* PoolableActor);

	UFUNCTION(BlueprintCallable, Category = "Pool Subsystem")
	void InitializePool(TSubclassOf<AActor> PoolClass, int32 MaxSize);

private:
	TMap<UClass*, FPoolArray> ObjectPools;

	AActor* GetActorFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation);

};
