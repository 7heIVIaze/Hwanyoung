// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ObjectMacros.h"
#include "HYCharacterMovementComponent.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_Climbing	UMETA(DisplayName = "Climbing"),
	CMOVE_Max		UMETA(Hidden),
};

UCLASS()
class HWANYOUNG2_API UHYCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
private:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SweepAndStoreWallHits();

	// Climb down from the top
	void SweepAndStoreTopHits();

	bool CanStartClimbing();

	// Climb down from the top
	bool CanStartClimbingFromTop();

	// Line trace from eye height for determining the wall can't be climbed
	bool EyeHeightTrace(const float TraceDistance) const;

	bool IsFacingSurface(const float SurfaceVerticalDot) const;

	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

	void PhysClimbing(float DeltaTime, int32 Iterations);
	
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	void ComputeSurfaceInfo();
	void ComputeClimbingVelocity(float DeltaTime);
	bool ShouldStopClimbing();
	void StopClimbing(float DeltaTime, int32 Iterations);
	void MoveAlongClimbingSurface(float DeltaTime);
	void SnapToClimbingSurface(float DeltaTime) const;

	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;

	FQuat GetClimbingRotation(float DeltaTime) const;

	// Down To Floor
	bool ClimbDownToFloor() const;
	bool CheckFloor(FHitResult& FloorHit) const;

	// Ledge
	bool TryClimbUpLedge();
	bool HasReachedEdge() const;
	bool IsLocationWalkable(const FVector& CheckLocation) const;
	bool CanMoveToLedgeClimbLocation() const;
	
	UFUNCTION()
	void ClimbUpLedgeFinished(UAnimMontage* Montage, bool bInterrupted);

	// Climb dash
	void StoreClimbDashDirection();
	void UpdateClimbDashState(float DeltaTime);
	void StopClimbDashing();
	void AlignClimbDashDirection();

	// Climb from the top
	bool TryClimbDownLedge();
	bool IsLocationHoldable(const FVector& CheckLocation) const;
	bool HasReachedLedge() const;
	bool CanMoveToClimbFromDownLocation() const;

	UFUNCTION()
	void ClimbDownLedgeFinished(UAnimMontage* Montage, bool bInterrupted);

public:
	void TryClimbing();

	void CancelClimbing();

	void TryClimbingFromTop();


	UFUNCTION(BlueprintCallable)
	bool IsClimbing() const;

	UFUNCTION(BlueprintCallable)
	FVector GetClimbSurfaceNormal() const;

	// Climb Dash
	UFUNCTION(BlueprintCallable)
	void TryClimbDashing();

	UFUNCTION(BlueprintPure)
	bool IsClimbDashing() const
	{
		return IsClimbing() && bIsClimbDashing;
	}

	UFUNCTION(BlueprintPure)
	FVector GetClimbDashDirection() const
	{
		return ClimbDashDirection;
	}

private:
	UPROPERTY(Category = "Character Movement : Climbing", EditAnywhere)
	int CollisionCapsuleRadius = 50;

	UPROPERTY(Category = "Character Movement : Climbing", EditAnywhere)
	int CollisionCapsuleHalfHeight = 72;

	UPROPERTY(Category = "Character Movement : Climbing", EditAnywhere, meta = (ClampMin = "1.0", ClampMax = "75.0"))
	float MinHorizontalDegreesToStartClimbing = 25.0f;

	UPROPERTY(Category = "Character Movement : Climbing", EditAnywhere, meta = (ClampMin = "8.0", ClampMax = "80.0"))
	float ClimbingCollisionShrinkAmount = 30.0f;

	UPROPERTY(Category = "Character Movement : Climbing", EditAnywhere, meta = (ClampMin = "10.0", ClampMax = "500.0"))
	float MaxClimbingSpeed = 125.0f;

	UPROPERTY(Category = "Character Movement : Climbing", EditAnywhere, meta = (ClampMin = "10.0", ClampMax = "2000.0"))
	float MaxClimbingAcceleration = 380.0f;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "3000.0"))
	float BrakingDecelerationClimbing = 550.f;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "1.0", ClampMax = "12.0"))
	int ClimbingRotationSpeed = 6;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "60.0"))
	float ClimbingSnapSpeed = 4.f;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "80.0"))
	float DistanceFromSurface = 45.f;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "1.0", ClampMax = "500.0"))
	float FloorCheckDistance = 100.f;

	// Ledging Variables
	UPROPERTY(Category = "Character Movement: Climbing", EditDefaultsOnly)
	UAnimMontage* LedgeClimbMontage;

	// Climb From the top Variables
	UPROPERTY(Category = "Character Movement: Climbing", EditDefaultsOnly)
	UAnimMontage* ClimbDownLedgeMontage;

	UPROPERTY()
	UAnimInstance* AnimInstance;

	UPROPERTY()
	class Ahwanyoung2Character* Player;

	// Climb Dash
	UPROPERTY(Category = "Character Movement: Climbing", EditDefaultsOnly)
	UCurveFloat* ClimbDashCurve;

	FVector ClimbDashDirection;
	bool bWantsToClimbDash = false;
	float CurrentClimbDashTime;
	bool bIsClimbDashing = false;

	bool bWantsToClimb = false;
	bool bWantsToClimbDown = false;

	FVector CurrentClimbingNormal;
	FVector CurrentClimbingPosition;

	TArray<FHitResult> CurrentWallHits;
	TArray<FHitResult> CurrentFloorHits;

	FCollisionQueryParams ClimbQueryParams;
};
