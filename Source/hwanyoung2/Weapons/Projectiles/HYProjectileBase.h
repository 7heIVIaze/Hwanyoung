// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/HYPoolableActor.h"
#include "HYProjectileBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProjectileImpact, AActor*, _HitActor, const FHitResult&, _HitResult);

UCLASS()
class HWANYOUNG2_API AHYProjectileBase : public AHYPoolableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHYProjectileBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma region EventGraphs
	UFUNCTION(BlueprintCallable)
	virtual void OnComponentBeginOverlap(UPrimitiveComponent* _OverlappedComp, AActor* _OtherActor, UPrimitiveComponent* _OtherComp, int32 _OtherIndexBody, bool _FromSweep, const FHitResult& _SweepResult);
#pragma endregion

#pragma region Interfaces
	// Get this actor from the object pool
	virtual void OnSpawnFromPool_Implementation();

	// Return to the object pool
	virtual void OnReturnToPool_Implementation();
#pragma endregion

#pragma region Functions
	// Life Span End
	UFUNCTION(BlueprintCallable)
	virtual void LifeSpanEnded();

	// Initialize projectile info
	UFUNCTION(BlueprintCallable)
	virtual void InitializeProjectile(AActor* _Owner, APawn* _Instigator, float _Speed, float _Gravity, float _Lifetime);

	// Shoot projectile
	UFUNCTION(BlueprintCallable)
	virtual void ShootProjectile();
#pragma endregion

#pragma region Getter
	UFUNCTION(BlueprintCallable)
	virtual UProjectileMovementComponent* GetProjectileMovement() { return ProjectileMovementComp; }
#pragma endregion

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBoxComponent> Box;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> TrailEffectLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UNiagaraComponent> TrailEffectRef;
	
	// Speed of projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	float Speed;

	// Gravity of projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	float Gravity;

	// Trail effect for firing projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UNiagaraSystem> TrailEffect;

	// Velocity of projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	FVector Velocity;

	// Impact VFX when shot
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UNiagaraSystem> ImpactVFX;

	// Life span timer to return to the pool
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	FTimerHandle ReturnTimer;

	// Life span time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	float LifeTime;

	// Projectile shooting sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UFMODEvent> ArrowSound;

	// Projectile hit sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UFMODEvent> HitSound;

public:
	// Projectile Impact Event Dispatcher
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnProjectileImpact OnProjectileImpact;
};
