// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HYProjectileBase.h"
#include "HYPlayerProjectileBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnArrowImpact, float, _Strength, AActor*, _HitActor, const FHitResult&, _HitResult);

UCLASS()
class HWANYOUNG2_API AHYPlayerProjectileBase : public AHYProjectileBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHYPlayerProjectileBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma region EventGraphs
	UFUNCTION(BlueprintNativeEvent)
	void Fire(FVector _Direction, float _Strength);

	// Fire the projectile
	virtual void Fire_Implementation(FVector _Direction, float _Strength);

	virtual void OnComponentBeginOverlap(UPrimitiveComponent* _OverlappedComp, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherIndexBody, bool _FromSweep, const FHitResult& _SweepResult) override;
#pragma endregion

#pragma region Interfaces
	// Get this actor from the object pool
	virtual void OnSpawnFromPool_Implementation();

	// Return to the object pool
	virtual void OnReturnToPool_Implementation();
#pragma endregion

protected:
	// Min speed of projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	float MinSpeed;

	// Max speed of projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	float MaxSpeed;

	// Min gravity of projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	float MinGravity;

	// Max gravity of projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	float MaxGravity;

	// Strength
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	float Strength;

public:
	// Arrow Impact Event Dispatcher
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnArrowImpact OnArrowImpact;
};
