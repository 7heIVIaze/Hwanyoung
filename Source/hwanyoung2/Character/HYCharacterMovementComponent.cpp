// // Copyright 2024. TEAM DAON. All rights reserved.

#include "HYCharacterMovementComponent.h"

#include "hwanyoung2Character.h"
#include "Components/CapsuleComponent.h"

#include "Kismet/KismetSystemLibrary.h"

void UHYCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<Ahwanyoung2Character>(GetCharacterOwner());
	AnimInstance = GetCharacterOwner()->GetMesh()->GetAnimInstance();

	ClimbQueryParams.AddIgnoredActor(GetOwner());
}

void UHYCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SweepAndStoreWallHits();
}

void UHYCharacterMovementComponent::SweepAndStoreWallHits()
{
	const FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CollisionCapsuleRadius, CollisionCapsuleHalfHeight);

	const FVector StartOffset = UpdatedComponent->GetForwardVector() * 20.0f;
	
	// Avoid using the same Start/End location for a Sweep, as it doesn't trigger hits on Landscapes.
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();

	TArray<FHitResult> Hits;
	const bool HitWall = GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity,
		ECC_WorldStatic, CollisionShape, ClimbQueryParams);

	if (HitWall)
	{
		CurrentWallHits = Hits;
		/*DrawDebugCapsule(GetWorld(), Start, CollisionCapsuleHalfHeight, CollisionCapsuleRadius,
			FQuat::Identity, FColor::Green);*/
	}
	else
	{
		CurrentWallHits.Reset();
		/*DrawDebugCapsule(GetWorld(), Start, CollisionCapsuleHalfHeight, CollisionCapsuleRadius,
			FQuat::Identity, FColor::Red);*/
	}
}

void UHYCharacterMovementComponent::SweepAndStoreTopHits()
{
	const FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CollisionCapsuleRadius, CollisionCapsuleHalfHeight);

	const FVector StartOffset = UpdatedComponent->GetForwardVector() * 20.0f;

	// Avoid using the same Start/End location for a Sweep, as it doesn't trigger hits on Landscapes.
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();

	TArray<FHitResult> Hits;
	const bool HitWall = GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity,
		ECC_WorldStatic, CollisionShape, ClimbQueryParams);

	if (HitWall)
	{
		CurrentWallHits = Hits;
		/*DrawDebugCapsule(GetWorld(), Start, CollisionCapsuleHalfHeight, CollisionCapsuleRadius,
			FQuat::Identity, FColor::Green);*/
	}
	else
	{
		CurrentWallHits.Reset();
		/*DrawDebugCapsule(GetWorld(), Start, CollisionCapsuleHalfHeight, CollisionCapsuleRadius,
			FQuat::Identity, FColor::Red);*/
	}
}

bool UHYCharacterMovementComponent::CanStartClimbing()
{
	for (FHitResult& Hit : CurrentWallHits)
	{
		const FVector HorizontalNormal = Hit.Normal.GetSafeNormal2D();

		const float HorizontalDot = FVector::DotProduct(UpdatedComponent->GetForwardVector(), -HorizontalNormal);
		const float VerticalDot = FVector::DotProduct(Hit.Normal, HorizontalNormal);

		const float HorizontalDegrees = FMath::RadiansToDegrees(FMath::Acos(HorizontalDot));

		const bool bIsCeiling = FMath::IsNearlyZero(VerticalDot);

		if (HorizontalDegrees <= MinHorizontalDegreesToStartClimbing && 
			!bIsCeiling && IsFacingSurface(VerticalDot))
		{
			return true;
		}
	}

	return false;
}

bool UHYCharacterMovementComponent::CanStartClimbingFromTop()
{
	for (FHitResult& Hit : CurrentWallHits)
	{
		const FVector HorizontalNormal = Hit.Normal.GetSafeNormal2D();

		const float HorizontalDot = FVector::DotProduct(UpdatedComponent->GetForwardVector(), -HorizontalNormal);
		const float VerticalDot = FVector::DotProduct(Hit.Normal, HorizontalNormal);

		const float HorizontalDegrees = FMath::RadiansToDegrees(FMath::Acos(HorizontalDot));

		const bool bIsCeiling = FMath::IsNearlyZero(VerticalDot);

		if (HorizontalDegrees <= MinHorizontalDegreesToStartClimbing &&
			!bIsCeiling && IsFacingSurface(VerticalDot))
		{
			return true;
		}
	}
	return false;
}

bool UHYCharacterMovementComponent::EyeHeightTrace(const float TraceDistance) const
{
	FHitResult UpperEdgeHit;
	
	const float BaseEyeHeight = GetCharacterOwner()->BaseEyeHeight;
	const float EyeHeightOffset = IsClimbing() ? BaseEyeHeight + ClimbingCollisionShrinkAmount : BaseEyeHeight;

	const FVector Start = UpdatedComponent->GetComponentLocation() + UpdatedComponent->GetUpVector() * EyeHeightOffset;
	const FVector End = Start + (UpdatedComponent->GetForwardVector() * TraceDistance);


	return GetWorld()->LineTraceSingleByChannel(UpperEdgeHit, Start, End, ECC_WorldStatic, ClimbQueryParams);
}

bool UHYCharacterMovementComponent::IsFacingSurface(const float SurfaceVerticalDot) const
{
	constexpr float BaseLength = 80.0f;
	const float SteepnessMultiplier = 1 + (1 - SurfaceVerticalDot) * 5;

	return EyeHeightTrace(BaseLength * SteepnessMultiplier);
}

void UHYCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	//if (CanStartClimbing())
	if(bWantsToClimb)
	{
		SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::CMOVE_Climbing);
	}

	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
}

void UHYCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	if (CustomMovementMode == ECustomMovementMode::CMOVE_Climbing)
	{
		PhysClimbing(DeltaTime, Iterations);
	}

	Super::PhysCustom(DeltaTime, Iterations);
}

void UHYCharacterMovementComponent::PhysClimbing(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	ComputeSurfaceInfo();

	if (ShouldStopClimbing() || ClimbDownToFloor())
	{
		StopClimbing(DeltaTime, Iterations);
		return;
	}

	UpdateClimbDashState(DeltaTime);

	ComputeClimbingVelocity(DeltaTime);

	const FVector OldLocation = UpdatedComponent->GetComponentLocation();

	MoveAlongClimbingSurface(DeltaTime);

	TryClimbUpLedge();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / DeltaTime;
	}

	SnapToClimbingSurface(DeltaTime);
}

void UHYCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	if (IsClimbing())
	{
		bOrientRotationToMovement = false;

		UCapsuleComponent* Capsule = GetCharacterOwner()->GetCapsuleComponent();
		Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() - ClimbingCollisionShrinkAmount);
		StopMovementImmediately();
	}

	const bool bWasClimbing = PreviousMovementMode == MOVE_Custom && PreviousCustomMode == CMOVE_Climbing;

	if (bWasClimbing)
	{
		bOrientRotationToMovement = true;

		const FRotator StandRotation = FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, 0);
		UpdatedComponent->SetRelativeRotation(StandRotation);

		UCapsuleComponent* Capsule = GetCharacterOwner()->GetCapsuleComponent();
		Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() + ClimbingCollisionShrinkAmount);

		// After exiting climbing mode, reset velocity and acceleration
		StopMovementImmediately();
	}

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void UHYCharacterMovementComponent::ComputeSurfaceInfo()
{
	CurrentClimbingNormal = FVector::ZeroVector;
	CurrentClimbingPosition = FVector::ZeroVector;

	if (CurrentWallHits.IsEmpty())
	{
		return;
	}

	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(6);

	for (const FHitResult& WallHits : CurrentWallHits)
	{
		const FVector End = Start + (WallHits.ImpactPoint - Start).GetSafeNormal() * 120.0f;

		FHitResult AssistHit;
		GetWorld()->SweepSingleByChannel(AssistHit, Start, End, FQuat::Identity,
			ECC_WorldStatic, CollisionSphere, ClimbQueryParams);

		CurrentClimbingPosition += AssistHit.ImpactPoint;
		CurrentClimbingNormal += AssistHit.Normal;
	}

	CurrentClimbingPosition /= CurrentWallHits.Num();
	CurrentClimbingNormal = CurrentClimbingNormal.GetSafeNormal();
}

void UHYCharacterMovementComponent::ComputeClimbingVelocity(float DeltaTime)
{
	RestorePreAdditiveRootMotionVelocity();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		if (bWantsToClimbDash)
		{
			AlignClimbDashDirection();

			const float CurrentCurveSpeed = ClimbDashCurve->GetFloatValue(CurrentClimbDashTime);
			Velocity = ClimbDashDirection * CurrentCurveSpeed;
		}
		else
		{
			constexpr float Friction = 0.0f;
			constexpr bool bFluid = false;
			CalcVelocity(DeltaTime, Friction, bFluid, BrakingDecelerationClimbing);
		}
	}

	ApplyRootMotionToVelocity(DeltaTime);
}

bool UHYCharacterMovementComponent::ShouldStopClimbing()
{
	const bool bIsOnCeiling = FVector::Parallel(CurrentClimbingNormal, FVector::UpVector);

	return !bWantsToClimb || CurrentClimbingNormal.IsZero() || bIsOnCeiling;
}

void UHYCharacterMovementComponent::StopClimbing(float DeltaTime, int32 Iterations)
{
	StopClimbDashing();

	bWantsToClimb = false;
	SetMovementMode(EMovementMode::MOVE_Falling);

	if (Player->GetWorldTimerManager().IsTimerActive(Player->StaminaReduceTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(Player->StaminaReduceTimer);
	}
	GetWorld()->GetTimerManager().SetTimer(Player->StaminaRechargeTimer, Player, &Ahwanyoung2Character::ReplenishStamina, 0.05f, true);

	StartNewPhysics(DeltaTime, Iterations);
}

void UHYCharacterMovementComponent::MoveAlongClimbingSurface(float DeltaTime)
{
	const FVector Adjusted = Velocity * DeltaTime;

	FHitResult Hit(1.0f);

	SafeMoveUpdatedComponent(Adjusted, GetClimbingRotation(DeltaTime), true, Hit);

	if (Hit.Time < 1.0f)
	{
		HandleImpact(Hit, DeltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.0f - Hit.Time), Hit.Normal, Hit, true);
	}
}

void UHYCharacterMovementComponent::SnapToClimbingSurface(float DeltaTime) const
{
	const FVector Forward = UpdatedComponent->GetForwardVector();
	const FVector Location = UpdatedComponent->GetComponentLocation();
	const FQuat Rotation = UpdatedComponent->GetComponentQuat();

	const FVector ForwardDifference = (CurrentClimbingPosition - Location).ProjectOnTo(Forward);
	const FVector Offset = -CurrentClimbingNormal * (ForwardDifference.Length() - DistanceFromSurface);

	constexpr bool bSweep = true;	
	const float SnapSpeed = ClimbingSnapSpeed * FMath::Max(1, Velocity.Length() / MaxClimbingSpeed);
	UpdatedComponent->MoveComponent(Offset * SnapSpeed * DeltaTime, Rotation, bSweep);
}

float UHYCharacterMovementComponent::GetMaxSpeed() const
{
	return IsClimbing() ? MaxClimbingSpeed : Super::GetMaxSpeed();
}

float UHYCharacterMovementComponent::GetMaxAcceleration() const
{
	return IsClimbing() ? MaxClimbingAcceleration : Super::GetMaxAcceleration();
}

FQuat UHYCharacterMovementComponent::GetClimbingRotation(float DeltaTime) const
{
	const FQuat Current = UpdatedComponent->GetComponentQuat();

	if (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
	{
		return Current;
	}

	const FQuat Target = FRotationMatrix::MakeFromX(-CurrentClimbingNormal).ToQuat();

	const float RotationSpeed = ClimbingRotationSpeed * FMath::Max(1, Velocity.Length() / MaxClimbingSpeed);
	return FMath::QInterpTo(Current, Target, DeltaTime, RotationSpeed);
}

bool UHYCharacterMovementComponent::ClimbDownToFloor() const
{
	FHitResult FloorHit;

	if (!CheckFloor(FloorHit))
	{
		return false;
	}

	const bool bOnWalkableFloor = FloorHit.Normal.Z > GetWalkableFloorZ();

	const float DownSpeed = FVector::DotProduct(Velocity, -FloorHit.Normal);
	const bool bIsMovingTowardsFloor = DownSpeed >= MaxClimbingSpeed / 3 && bOnWalkableFloor;

	const bool bIsClimbingFloor = CurrentClimbingNormal.Z > GetWalkableFloorZ();

	return bIsMovingTowardsFloor || (bIsClimbingFloor && bOnWalkableFloor);
}

bool UHYCharacterMovementComponent::CheckFloor(FHitResult& FloorHit) const
{
	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FVector End = Start + FVector::DownVector * FloorCheckDistance;

	return GetWorld()->LineTraceSingleByChannel(FloorHit, Start, End, ECC_WorldStatic, ClimbQueryParams);
}

bool UHYCharacterMovementComponent::TryClimbUpLedge()
{
	if (AnimInstance && LedgeClimbMontage && AnimInstance->Montage_IsPlaying(LedgeClimbMontage))
	{
		return false;
	}

	const float UpSpeed = FVector::DotProduct(Velocity, UpdatedComponent->GetUpVector());
	const bool bIsMovingUp = UpSpeed >= MaxClimbingSpeed / 3;
	
	if (bIsMovingUp && HasReachedEdge() && CanMoveToLedgeClimbLocation())
	{
		if (AnimInstance->OnMontageEnded.IsBound())
		{
			AnimInstance->OnMontageEnded.RemoveDynamic(this, &UHYCharacterMovementComponent::ClimbUpLedgeFinished);
		}
		AnimInstance->OnMontageEnded.AddDynamic(this, &UHYCharacterMovementComponent::ClimbUpLedgeFinished);

		const FRotator StandRotation = FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, 0);
		UpdatedComponent->SetRelativeRotation(StandRotation);

		AnimInstance->Montage_Play(LedgeClimbMontage);

		return true;
	}

	return false;
}

bool UHYCharacterMovementComponent::HasReachedEdge() const
{
	const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	const float TraceDistance = Capsule->GetUnscaledCapsuleRadius() * 2.5f;

	return !EyeHeightTrace(TraceDistance);
}

bool UHYCharacterMovementComponent::IsLocationWalkable(const FVector& CheckLocation) const
{
	const FVector CheckEnd = CheckLocation + (FVector::DownVector * 250.f);

	FHitResult LedgeHit;
	const bool bHitLedgeGround = GetWorld()->LineTraceSingleByChannel(LedgeHit, CheckLocation, CheckEnd,
		ECC_WorldStatic, ClimbQueryParams);

	return bHitLedgeGround && LedgeHit.Normal.Z >= GetWalkableFloorZ();
}

bool UHYCharacterMovementComponent::CanMoveToLedgeClimbLocation() const
{
	const FVector VerticalOffset = FVector::UpVector * 160.f;
	const FVector HorizontalOffset = UpdatedComponent->GetForwardVector() * 120.f;

	const FVector CheckLocation = UpdatedComponent->GetComponentLocation() + HorizontalOffset + VerticalOffset;

	if (!IsLocationWalkable(CheckLocation))
	{
		return false;
	}

	FHitResult CapsuleHit;
	const FVector CapsuleStartCheck = CheckLocation - HorizontalOffset;
	const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();

	const bool bBlocked = GetWorld()->SweepSingleByChannel(CapsuleHit, CapsuleStartCheck, CheckLocation,
		FQuat::Identity, ECC_WorldStatic, Capsule->GetCollisionShape(), ClimbQueryParams);

	return !bBlocked;
}

void UHYCharacterMovementComponent::ClimbUpLedgeFinished(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Warning, TEXT("Climb Finished Called"));
	if (Montage == LedgeClimbMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("LedgeClimbMontage!"));
		if (Player->GetWorldTimerManager().IsTimerActive(Player->StaminaReduceTimer))
		{
			GetWorld()->GetTimerManager().ClearTimer(Player->StaminaReduceTimer);
		}
		GetWorld()->GetTimerManager().SetTimer(Player->StaminaRechargeTimer, Player, &Ahwanyoung2Character::ReplenishStamina, 0.05f, true);

		AnimInstance->OnMontageEnded.RemoveDynamic(this, &UHYCharacterMovementComponent::ClimbDownLedgeFinished);
	}
}

void UHYCharacterMovementComponent::StoreClimbDashDirection()
{
	ClimbDashDirection = UpdatedComponent->GetUpVector();

	const float AccelerationThreshold = MaxClimbingAcceleration / 10;
	if (Acceleration.Length() > AccelerationThreshold)
	{
		ClimbDashDirection = Acceleration.GetSafeNormal();
	}
}

void UHYCharacterMovementComponent::UpdateClimbDashState(float DeltaTime)
{
	if (!bWantsToClimbDash)
	{
		return;
	}

	CurrentClimbDashTime += DeltaTime;

	// Better to cache it when dash starts
	float MinTime, MaxTime;
	ClimbDashCurve->GetTimeRange(MinTime, MaxTime);

	if (CurrentClimbDashTime >= MaxTime)
	{
		StopClimbDashing();
	}
}

void UHYCharacterMovementComponent::StopClimbDashing()
{
	bWantsToClimbDash = false;
	CurrentClimbDashTime = 0.f;
}

void UHYCharacterMovementComponent::AlignClimbDashDirection()
{
	const FVector HorizontalSurfaceNormal = GetClimbSurfaceNormal().GetSafeNormal2D();

	ClimbDashDirection = FVector::VectorPlaneProject(ClimbDashDirection, HorizontalSurfaceNormal);
}

bool UHYCharacterMovementComponent::TryClimbDownLedge()
{
	if (AnimInstance && ClimbDownLedgeMontage && AnimInstance->Montage_IsPlaying(ClimbDownLedgeMontage))
	{
		return false;
	}

	//if (IsMovingOnGround() && HasReachedEdge() && CanMoveToClimbFromDownLocation())
	if (CurrentFloor.IsWalkableFloor() && CanMoveToClimbFromDownLocation())
	{
		SetMovementMode(EMovementMode::MOVE_Flying);
		StopMovementImmediately();

		if (AnimInstance->OnMontageEnded.IsBound())
		{
			AnimInstance->OnMontageEnded.RemoveDynamic(this, &UHYCharacterMovementComponent::ClimbDownLedgeFinished);
		}
		AnimInstance->OnMontageEnded.AddDynamic(this, &UHYCharacterMovementComponent::ClimbDownLedgeFinished);
		const FRotator StandRotation = FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, 0);
		UpdatedComponent->SetRelativeRotation(StandRotation);
		AnimInstance->Montage_Play(ClimbDownLedgeMontage);
		
		
		return true;
	}

	return false;
}

bool UHYCharacterMovementComponent::HasReachedLedge() const
{
	/*onst FVector CheckEnd = CheckLocation + (FVector::DownVector * 150.f);

	FHitResult LedgeHit;
	const bool bHitLedgeGround = GetWorld()->LineTraceSingleByChannel(LedgeHit, CheckLocation, CheckEnd,
		ECC_WorldStatic, ClimbQueryParams);

	return bHitLedgeGround && LedgeHit.Normal.Z >= GetWalkableFloorZ();*/
	return false;
}

bool UHYCharacterMovementComponent::IsLocationHoldable(const FVector& CheckLocation) const
{
	const FVector CheckEnd = CheckLocation + (FVector::DownVector * 150.f);

	FHitResult LedgeHit;
	const bool bHitLedgeGround = GetWorld()->LineTraceSingleByChannel(LedgeHit, CheckLocation, CheckEnd,
		ECC_WorldStatic, ClimbQueryParams);

	/*if (bHitLedgeGround)
	{
		DrawDebugLine(GetWorld(), CheckLocation, CheckEnd, FColor::Green, false, 10.0f);
	}
	else
	{
		DrawDebugLine(GetWorld(), CheckLocation, CheckEnd, FColor::Red, false, 10.0f);
		UE_LOG(LogTemp, Log, TEXT("IsLocationHoldable return false"));
	}*/

	return bHitLedgeGround && LedgeHit.Normal.Z >= GetWalkableFloorZ();
	//const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	//const float TraceDistance = Capsule->GetUnscaledCapsuleHalfHeight() * 2.5f;

	//FHitResult LowerEdgeHit;

	//const FVector Start = UpdatedComponent->GetComponentLocation() + UpdatedComponent->GetForwardVector();
	//const FVector End = Start + (UpdatedComponent->GetForwardVector() * TraceDistance);


	//return !GetWorld()->LineTraceSingleByChannel(LowerEdgeHit, Start, End, ECC_WorldStatic, ClimbQueryParams);
}

bool UHYCharacterMovementComponent::CanMoveToClimbFromDownLocation() const
{
	UE_LOG(LogTemp, Log, TEXT("CanMoveToClimbFromDownLocation"));
	const FVector VerticalOffset = FVector::DownVector * 160.f;
	const FVector HorizontalOffset = UpdatedComponent->GetForwardVector() * 120.f;

	const FVector CheckLocation = UpdatedComponent->GetComponentLocation() + HorizontalOffset + VerticalOffset;

	// If the character is not reached ledge
	if (IsLocationHoldable(CheckLocation))
	{
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("IsLocationHoldable Passed"));

	FHitResult CapsuleHit;
	const FVector CapsuleStartCheck = CheckLocation + HorizontalOffset / 4.0f;
	const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	TArray< TEnumAsByte< EObjectTypeQuery > > Actors;
	Actors.Add(EObjectTypeQuery::ObjectTypeQuery1);
		
	const bool bBlocked = GetWorld()->SweepSingleByChannel(CapsuleHit, CapsuleStartCheck, CheckLocation,
		FQuat::Identity, ECC_WorldStatic, Capsule->GetCollisionShape(), ClimbQueryParams);

	/*if (bBlocked)
	{
		DrawDebugCapsule(GetWorld(), (CheckLocation + CapsuleStartCheck) / 2.0f, Capsule->GetScaledCapsuleHalfHeight(),
			Capsule->GetScaledCapsuleRadius(), FQuat::Identity, FColor::Green, false, 10.0f);
	}
	else
	{
		DrawDebugCapsule(GetWorld(), (CheckLocation + CapsuleStartCheck) / 2.0f, Capsule->GetScaledCapsuleHalfHeight(),
			Capsule->GetScaledCapsuleRadius(), FQuat::Identity, FColor::Red, false, 10.0f);
	}*/

	return !bBlocked;
}

void UHYCharacterMovementComponent::ClimbDownLedgeFinished(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == ClimbDownLedgeMontage)
	{
		bWantsToClimb = true;
		Player->ToggleStaminaBar(true);

		if (Player->GetWorldTimerManager().IsTimerActive(Player->StaminaRechargeTimer))
		{
			GetWorld()->GetTimerManager().ClearTimer(Player->StaminaRechargeTimer);
		}
		GetWorld()->GetTimerManager().SetTimer(Player->StaminaReduceTimer, Player, &Ahwanyoung2Character::UseStamina, 0.05f, true);

		AnimInstance->OnMontageEnded.RemoveDynamic(this, &UHYCharacterMovementComponent::ClimbDownLedgeFinished);
	}
}

void UHYCharacterMovementComponent::TryClimbing()
{
	if (CanStartClimbing())
	{
		bWantsToClimb = true;

		Player->ToggleStaminaBar(true);

		if (Player->GetWorldTimerManager().IsTimerActive(Player->StaminaRechargeTimer))
		{
			GetWorld()->GetTimerManager().ClearTimer(Player->StaminaRechargeTimer);
		}
		GetWorld()->GetTimerManager().SetTimer(Player->StaminaReduceTimer, Player, &Ahwanyoung2Character::UseStamina, 0.05f, true);
	}
}

void UHYCharacterMovementComponent::CancelClimbing()
{
	bWantsToClimb = false;

	if (Player->GetWorldTimerManager().IsTimerActive(Player->StaminaReduceTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(Player->StaminaReduceTimer);
	}
	GetWorld()->GetTimerManager().SetTimer(Player->StaminaRechargeTimer, Player, &Ahwanyoung2Character::ReplenishStamina, 0.05f, true);
}

void UHYCharacterMovementComponent::TryClimbingFromTop()
{
	UE_LOG(LogTemp, Log, TEXT("TryClimbingFromTop"));
	TryClimbDownLedge();
}

bool UHYCharacterMovementComponent::IsClimbing() const
{
	return MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == ECustomMovementMode::CMOVE_Climbing;
}

FVector UHYCharacterMovementComponent::GetClimbSurfaceNormal() const
{
	return CurrentClimbingNormal;
}

void UHYCharacterMovementComponent::TryClimbDashing()
{
	if (ClimbDashCurve && bWantsToClimbDash == false)
	{
		bWantsToClimbDash = true;
		CurrentClimbDashTime = 0.f;

		StoreClimbDashDirection();
	}
}
