// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Poolable.h"
#include "HYPoolableActor.generated.h"

UCLASS()
class HWANYOUNG2_API AHYPoolableActor : public AActor, public IPoolable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHYPoolableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	bool bIsActive;

	// Get this actor from the object pool
	virtual void OnSpawnFromPool_Implementation();

	// Return to the object pool
	virtual void OnReturnToPool_Implementation();

	bool GetActivate() const { return bIsActive; }
};
