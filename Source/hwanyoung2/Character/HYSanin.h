// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HYAggroNPCBase.h"
#include "HYSanin.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API AHYSanin : public AHYAggroNPCBase
{
	GENERATED_BODY()

public:
#pragma region Event Graphs
	// Gong gi Nori(Default Projectile Attack) - BlueprintNativeEvent is for SFX
	// This is just spawn the gonggi projectiles
	UFUNCTION(BlueprintNativeEvent)
	void GonggiNori();

	virtual void GonggiNori_Implementation();

	// Gong gi Nori(Default Projectile Attack) - BlueprintNativeEvent is for SFX
	// This is really shoot gonggi.
	// @param Radian = Fired Degree, FiredAtTarget = Is this stone aimed at target?
	UFUNCTION(BlueprintNativeEvent)
	void ShootGonggiStone(float _Radian = 0.0f, bool _FiredAtTarget = true);

	virtual void ShootGonggiStone_Implementation(float _Radian = 0.0f, bool _FiredAtTarget = true);

#pragma endregion

#pragma region Functions
	UFUNCTION(BlueprintCallable)
	void GonggiDamage(AActor* _OtherActor, const FHitResult& _Hit);
#pragma endregion
	
#pragma region Variables
	// How many rocks can be shoot for gonggi nori.
	// Combo stack
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack|Melee")
	int EnableRockShoot = 1;

	// Rock stack. it increase combo stack (5 => 2, 10 => 3, 17 => 4)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack|Melee")
	int CurrentRockStack = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack|Melee")
	int FirstRockShoot = 1;

	// Gonggi Stone projectile class
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack|Melee")
	TSubclassOf<AActor> GonggiClass;
#pragma endregion
};
