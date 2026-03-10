// // Copyright 2024. TEAM DAON. All rights reserved.

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
		UE_LOG(LogTemp, Log, TEXT("Push Object from the pool"));
		ObjectPool.Add(PoolableActor);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Destroy"));
		PoolableActor->Destroy();
	}
}

void UHYPoolSubSystem::InitializePool(TSubclassOf<AActor> PoolClass, int32 MaxSize, APawn* InstigatorPawn)
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
			NewActor->SetInstigator(InstigatorPawn);
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
			UE_LOG(LogTemp, Log, TEXT("Pop Object from the pool"));
			Actor->SetActorHiddenInGame(false);
			Actor->SetActorEnableCollision(true);
			return Actor;
		}
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* NewActor = GetWorld()->SpawnActor<AActor>(
		PoolClass, Location, Rotation, SpawnParams);
	UE_LOG(LogTemp, Log, TEXT("Spawn Object"));


	if (NewActor && PoolClass.Get()->ImplementsInterface(UPoolable::StaticClass()))
	{
		IPoolable::Execute_OnSpawnFromPool(NewActor);
	}
	return NewActor;
}
