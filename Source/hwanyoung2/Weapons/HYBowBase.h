// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HYWeaponBase.h"
#include "HYBowBase.generated.h"

UENUM(BlueprintType)
enum class EBowState : uint8
{
	Idle UMETA(DisplayName = "Idle"),			// Idle State
	Aim UMETA(DisplayName = "Aim"),			// Aiming State
	Draw UMETA(DisplayName = "Draw"),			// Drawing State
};

UCLASS()
class HWANYOUNG2_API AHYBowBase : public AHYWeaponBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHYBowBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma region EventGraphs
	// Drawing bow
	UFUNCTION(BlueprintNativeEvent)
	void DrawBegin();

	// Drawing bow
	virtual void DrawBegin_Implementation();

	// Stop drawing bow
	UFUNCTION(BlueprintNativeEvent)
	void DrawEnd();

	// Stop drawing bow
	virtual void DrawEnd_Implementation();
#pragma endregion

#pragma region Getter
	UFUNCTION(BlueprintCallable)
	EBowState GetBowState() { return BowState; }

	UFUNCTION(BlueprintCallable)
	float GetMaxDrawTime() { return MaxDrawTime; }
#pragma endregion

#pragma region Setter
	UFUNCTION(BlueprintCallable)
	void SetBowState(EBowState _BowState) { BowState = _BowState; }

#pragma endregion
private:
	// Bow State
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "State", meta = (AllowPrivateAccess = "true"))
	EBowState BowState;

	// Draw time for increasing projectile damage
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	float MaxDrawTime;
};
