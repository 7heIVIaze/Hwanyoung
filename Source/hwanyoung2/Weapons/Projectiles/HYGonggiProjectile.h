// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HYProjectileBase.h"
#include "Components/TimelineComponent.h"
#include "HYGonggiProjectile.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API AHYGonggiProjectile : public AHYProjectileBase
{
	GENERATED_BODY()
	
#pragma region Event Graphs
protected:
	virtual void BeginPlay() override;

public:
	AHYGonggiProjectile();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void RotateToTarget();

	UFUNCTION(BlueprintCallable)
	void ShootStraight();
#pragma endregion

#pragma region Functions
	// Get this actor from the object pool
	virtual void OnSpawnFromPool_Implementation();

	// Return to the object pool
	virtual void OnReturnToPool_Implementation();

	// Initialize the member variables of the gonggi projectile
	UFUNCTION(BlueprintCallable)
	void InitializeActor(AActor* _Owner, APawn* _Instigator, AActor* _TargetActor, bool _IsAimedAtTarget, float _Speed, float _Gravity, float _Lifetime);

	// Shoot the gonggi projectile
	virtual void ShootProjectile() override;

	// Retrieving this Gonggi for the ultimate skill
	UFUNCTION(BlueprintCallable)
	void RetrieveGonggi(AActor* _SourceActor, FVector _Location);

	// Timeline update function
	UFUNCTION(BlueprintCallable)
	void RetrieveUpdated(float _Speed);

	// Timeline finished function
	UFUNCTION(BlueprintCallable)
	void RetrieveFinished();
#pragma endregion

#pragma region Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectile")
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectile")
	bool bIsAimedAtTarget;

	FTimeline RetrieveTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	UCurveFloat* SpeedCurveFloat;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Timeline")
	FVector StartLocation;
#pragma endregion

#pragma region Components
	TObjectPtr<class UNiagaraComponent> Niagara;
#pragma endregion
};
