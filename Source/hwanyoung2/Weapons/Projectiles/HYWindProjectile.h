// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HYProjectileBase.h"
#include "HYWindProjectile.generated.h"

UCLASS()
class HWANYOUNG2_API AHYWindProjectile : public AHYProjectileBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHYWindProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma region Interfaces
	// Get this actor from the object pool
	virtual void OnSpawnFromPool_Implementation();

	// Return to the object pool
	virtual void OnReturnToPool_Implementation();
#pragma endregion

#pragma region Functions

	// Initialize the member variables of the wind projectile
	UFUNCTION(BlueprintCallable)
	void InitializeActor(AActor* _Owner, APawn* _Instigator, AActor* _TargetActor, float _Gravity, float _Lifetime, float _MinSpeed, float _MaxSpeed, bool _IsRandom);

	// Shoot projectile
	virtual void ShootProjectile() override;
#pragma endregion

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variables")
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variables")
	float MinSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variables")
	float MaxSpeed;

	// Life span time
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variables")
	bool bIsRandom;
};
