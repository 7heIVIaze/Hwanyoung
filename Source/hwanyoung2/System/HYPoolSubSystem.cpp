// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYPoolSubSystem.h"
#include "Interfaces/Poolable.h"

void UHYPoolSubSystem::SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation, AActor*& SpawnedActor)
{
	SpawnedActor = GetActorFromPool(PoolClass, Location, Rotation);
}

void UHYPoolSubSystem::ReturnToPool(AActor* PoolableActor)
{
	if (!PoolableActor)
	{
		return;
	}

	UClass* ActorClass = PoolableActor->GetClass();

	if (ActorClass->ImplementsInterface(UPoolable::StaticClass()))
	{
		IPoolable::Execute_OnReturnToPool(PoolableActor);
		PoolableActor->SetActorHiddenInGame(true);
		PoolableActor->SetActorEnableCollision(false);
		FPoolArray& ObjectPool = ObjectPools.FindOrAdd(ActorClass);
		ObjectPool.Add(PoolableActor);
	}
	else
	{
		PoolableActor->Destroy();
	}
}

void UHYPoolSubSystem::InitializePool(TSubclassOf<AActor> PoolClass, int32 MaxSize)
{
	FPoolArray& ObjectPool = ObjectPools.FindOrAdd(PoolClass);

	for (int32 i = 0; i < MaxSize; ++i)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AActor* NewActor = GetWorld()->SpawnActor<AActor>(
			PoolClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);


		if (NewActor && PoolClass.Get()->ImplementsInterface(UPoolable::StaticClass()))
		{
			IPoolable::Execute_OnReturnToPool(NewActor);
			NewActor->SetActorHiddenInGame(true);
			NewActor->SetActorEnableCollision(false);
			ObjectPool.Add(NewActor);
		}
	}
}

AActor* UHYPoolSubSystem::GetActorFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation)
{
	FPoolArray& ObjectPool = ObjectPools.FindOrAdd(PoolClass);

	if (!ObjectPool.IsEmpty())
	{
		AActor* Actor = ObjectPool.Pop();

		if (Actor)
		{
			IPoolable::Execute_OnSpawnFromPool(Actor);
			Actor->SetActorLocationAndRotation(Location, Rotation);
			//Actor->SetActorHiddenInGame(false);
			return Actor;
		}
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* NewActor = GetWorld()->SpawnActor<AActor>(
		PoolClass, Location, Rotation, SpawnParams);


	if (NewActor && PoolClass.Get()->ImplementsInterface(UPoolable::StaticClass()))
	{
		IPoolable::Execute_OnSpawnFromPool(NewActor);
	}
	return NewActor;
}
