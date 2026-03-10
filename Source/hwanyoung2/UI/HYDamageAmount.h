// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/HYPoolableActor.h"
#include "Components/Timelinecomponent.h"
#include "HYDamageAmount.generated.h"

UCLASS()
class HWANYOUNG2_API AHYDamageAmount : public AHYPoolableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHYDamageAmount();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

#pragma region Interfaces
	// Get this actor from the object pool
	virtual void OnSpawnFromPool_Implementation() override;

	// Return to the object pool
	virtual void OnReturnToPool_Implementation() override;
#pragma endregion

#pragma region Functions
	UFUNCTION(BlueprintCallable)
	void OnDamageAmountMoveUpdate(float MoveTrack);

	UFUNCTION(BlueprintCallable)
	void OnDamageAmountMoveEnd();

	// Initialize
	UFUNCTION(BlueprintCallable)
	void Initialize(float inDamageAmount, EDamageType inDamageType);

#pragma endregion

#pragma region Components
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	// Damage amount widget
	TObjectPtr<class UWidgetComponent> DamageWidget;
#pragma endregion

public:
#pragma region Variables
	// for 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widget")
	TObjectPtr<UUserWidget> DamageAmountWidget;

	// Damage amount
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
	float DamageAmount;

	// for animation of damage amount up and down
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
	FVector StartLocation;

	// for animation of damage amount up and down
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
	FVector EndLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
	EDamageType DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Timeline")
	UCurveFloat* MoveTimelineCurve;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Timeline")
	FTimeline MoveTimeline;
#pragma endregion
};
