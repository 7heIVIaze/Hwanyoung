// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HYMPSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnManaUsed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnManaReplenished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUltimatePointsUsed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUltimatePointsAdded);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HWANYOUNG2_API UHYMPSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHYMPSystem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region Functions
	// MP Recovery function.
	UFUNCTION(BlueprintCallable)
	float ReplenishMana(float _AmountOfMana);

	// Use Mana for casting skill
	UFUNCTION(BlueprintCallable)
	bool UseMana(float _ToBeConsumedMana);

	// Use Ult points for casting ultimate skill
	UFUNCTION(BlueprintCallable)
	bool UseUltimatePoints(float _ConsumedUltPoints);

	UFUNCTION(BlueprintCallable)
	float AddUltimatePoints(float _AddedUltPoints);

	// MP Recovery function.
	UFUNCTION(BlueprintCallable)
	bool ReplenishStamina(float _AmountOfStamina);

	// Use Mana for casting skill
	UFUNCTION(BlueprintCallable)
	bool UseStamina(float _ToBeConsumedStamina);

#pragma region Buffs
	// Apply MP buff
	UFUNCTION(BlueprintCallable, Category = "Buff")
	bool Buff(float _AmountToBuffed);

	// Remove MP buff
	UFUNCTION(BlueprintCallable, Category = "Buff")
	bool RemoveBuff(float _AmountToBuffed);

#pragma endregion

#pragma endregion

#pragma region Setter/Getter
	UFUNCTION(BlueprintCallable)
	float GetCurrentMana() { return CurrentMana; }

	UFUNCTION(BlueprintCallable)
	float GetMaxMana() { return MaxMana; }

	UFUNCTION(BlueprintCallable)
	float GetCurrentUltimatePoint() { return CurrentUltimatePoint; }

	UFUNCTION(BlueprintCallable)
	float GetMaxUltimatePoint() { return MaxUltimatePoint; }

	UFUNCTION(BlueprintCallable)
	float GetCurrentStamina() { return CurrentStamina; }

	UFUNCTION(BlueprintCallable)
	float GetMaxStamina() { return MaxStamina; }
#pragma endregion

#pragma region Variables
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP", meta = (AllowPrivateAccess = "true"))
	float CurrentMana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP", meta = (AllowPrivateAccess = "true"))
	float MaxMana;

	// Current Ultimate Point for using ultimate skill.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP", meta = (AllowPrivateAccess = "true"))
	float CurrentUltimatePoint;

	// Max ultimate points
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP", meta = (AllowPrivateAccess = "true"))
	float MaxUltimatePoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP", meta = (AllowPrivateAccess = "true"))
	float CurrentStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP", meta = (AllowPrivateAccess = "true"))
	float MaxStamina;
#pragma endregion

public:
#pragma region Event Dispatcher
	// MP used event dispatcher
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnManaUsed OnManaUsed;

	// MP replenish event dispatcher
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnManaReplenished OnManaReplenished;

	// Ult points used event dispatcher
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnUltimatePointsUsed OnUltimatePointsUsed;

	// Ult points added event dispatcher
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnUltimatePointsAdded OnUltimatePointsAdded;
#pragma endregion
};
