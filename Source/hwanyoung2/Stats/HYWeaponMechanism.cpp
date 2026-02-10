// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYWeaponMechanism.h"
#include "Weapons/HYWeaponBase.h"
#include "Weapons/HYBowBase.h"
#include "Weapons/Projectiles/HYPlayerProjectileBase.h"
#include "Character/HYPlayerCharacterBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Camera/CameraComponent.h"
#include "HYAttributeSystem.h"
#include "Character/HYEnemyBase.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Damage.h"
#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Character/HYAggroNPCBase.h"

// Sets default values for this component's properties
UHYWeaponMechanism::UHYWeaponMechanism()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHYWeaponMechanism::BeginPlay()
{
	Super::BeginPlay();

	// ...
	PlayerRef = Cast<AHYPlayerCharacterBase>(GetOwner());
	EquipWeapon(PlayerRef->GetWeaponIndex());
	OriginalRotationRate = PlayerRef->GetCharacterMovement()->RotationRate;
	BowRef = Cast<AHYBowBase>(WeaponRef);
}


// Called every frame
void UHYWeaponMechanism::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Do only weapon trace is on.
	if (bWeaponTrace)
	{
		PerformWeaponTrace();
	}
}

void UHYWeaponMechanism::EquipWeapon(int _WeaponIndex)
{
	switch (_WeaponIndex)
	{
		// Close combat
		case 0:
		{
			WeaponClass = CloseCombatWeaponClass;
			WeaponSocketName = FName("None");
		}
		break;
		// Melee weapon
		case 1:
		{
			WeaponClass = MeleeCombatWeaponClass;
			WeaponSocketName = FName("MeleeWeaponSocket");
		}
		break;
		// Projectile weapon
		case 2:
		{
			WeaponClass = ProjectileWeaponClass;
			WeaponSocketName = FName("ProjectileWeaponSocket");
		}
		break;
		// Default
		default:
			WeaponClass = CloseCombatWeaponClass;
			WeaponSocketName = FName("None");
	}
	
	FActorSpawnParameters spawnParams;
	FRotator rotator;
	FAttachmentTransformRules attachmentRules(
		EAttachmentRule::KeepRelative,
		EAttachmentRule::KeepRelative,
		EAttachmentRule::SnapToTarget,
		true
	);

	// Spawn weapon and attach to weapon socket
	WeaponRef = GetWorld()->SpawnActor<AHYWeaponBase>(WeaponClass, FVector::ZeroVector, rotator, spawnParams);
	WeaponRef->AttachToComponent(PlayerRef->GetMesh(), attachmentRules, WeaponSocketName);

}

void UHYWeaponMechanism::UnequipWeapon()
{
	if (nullptr != WeaponRef)
	{
		WeaponRef->Destroy();
	}
}

void UHYWeaponMechanism::SpawnArrow()
{
	// Spawn projectile and attach to weapon
	FActorSpawnParameters spawnParams;
	ArrowRef = GetWorld()->SpawnActor<AHYPlayerProjectileBase>(ProjectileClass, 
		FVector::ZeroVector, FRotator::ZeroRotator, spawnParams);
	ArrowRef->SetInstigator(PlayerRef);

	ArrowRef->AttachToComponent(PlayerRef->GetMesh(), 
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), ProjectileSocketName);

}

void UHYWeaponMechanism::DestroyArrow()
{
	if (nullptr != ArrowRef)
	{
		ArrowRef->Destroy();
	}
}

void UHYWeaponMechanism::FireAimedArrow()
{
	if (nullptr != ArrowRef)
	{
		// Detach arrow from player to shoot.
		ArrowRef->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		if (nullptr != BowRef)
		{
			// Calculate direction and strength
			FVector vDirection = CalculateAimDirection();
			float fStrength = UKismetMathLibrary::NormalizeToRange(DrawTime, 0.0f, BowRef->GetMaxDrawTime());
			fStrength = FMath::Clamp(fStrength, 0.0f, 1.0f);

			// Fire the arrow
			ArrowRef->Fire_Implementation(vDirection, fStrength);

			ArrowRef = nullptr;
		}
	}
}

FVector UHYWeaponMechanism::CalculateAimDirection()
{
	FVector vDirection;
	if (nullptr != PlayerRef)
	{
		UCameraComponent* Camera = PlayerRef->GetFollowCamera();

		// Set up vectors for line trace
		FVector Start = Camera->GetComponentLocation();
		FVector Forward = Camera->GetForwardVector();

		FVector End = Start + Forward * 1000000.0f;
		FHitResult OutHit;

		// Line trace for calculate direction.
		bool bResult = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility);

		// Calcuate direction for arrow
		if (nullptr != ArrowRef)
		{
			FVector From = ArrowRef->GetActorLocation();
			FVector To = bResult ? OutHit.Location : OutHit.TraceEnd;

			vDirection = UKismetMathLibrary::GetDirectionUnitVector(From, To);
		}
	}

	return vDirection;
}

bool UHYWeaponMechanism::CalculateProjectileDamage(float _Strength, float& OutGeneratedDamage)
{
	bool bIsCritical = false;
	if (nullptr != WeaponRef)
	{
		UHYAttributeSystem* AttributeSystem = WeaponRef->GetAttributeSystem();

		// Determines projectile damage based on critical hit rate.
		float fProbability = 100.0f - FMath::RandRange(0.0f, 100.0f);
		bIsCritical = fProbability < AttributeSystem->GetCurrentCriticalRate();
		float fCurrentDamage = (bIsCritical) ? AttributeSystem->GetCurrentCriticalDamage() : AttributeSystem->GetCurrentPhysicalDamage();

		// To make the normal damage 150, multiply value.
		OutGeneratedDamage = _Strength * 0.0005f * fCurrentDamage;
	}

	return bIsCritical;
}

void UHYWeaponMechanism::PerformWeaponTrace()
{
	if (false != IsValid(WeaponRef))
	{
		USkeletalMeshComponent* WeaponMesh = WeaponRef->GetWeaponMesh();

		// Set up values for box trace.
		FVector CurrentTopTrace = WeaponMesh->GetSocketLocation(TEXT("TopTrace"));
		FVector CurrentBottomTrace = WeaponMesh->GetSocketLocation(TEXT("BottomTrace"));
		TArray<FHitResult> OutHits;
		float fHalfSizeX = UKismetMathLibrary::Vector_Distance(CurrentTopTrace, CurrentBottomTrace) / 2.0f;
		FRotator Orientation = UKismetMathLibrary::FindLookAtRotation(CurrentTopTrace, CurrentBottomTrace);
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectType = { EObjectTypeQuery::ObjectTypeQuery3 };

		// Box trace for getting hit actors
		bool bHitResult = UKismetSystemLibrary::BoxTraceMultiForObjects(this, PreviousTopSocket, CurrentTopTrace,
			FVector(fHalfSizeX, 15.0f, 30.0f), Orientation, ObjectType, false,
			TArray<AActor*>(), EDrawDebugTrace::None, OutHits, true);

		// update the location of sockets
		PreviousTopSocket = CurrentTopTrace;
		PreviousBottomSocket = CurrentBottomTrace;

		for (auto OutHit : OutHits)
		{
			// if hit success
			if (true == OutHit.bBlockingHit)
			{
				AActor *HitActor = OutHit.GetActor();
				// If the hit actor is the first identified actor
				if (false == AlreadyDamaged.Contains(HitActor))
				{
					// Insert the actor in set.
					AlreadyDamaged.Add(HitActor);
					float GeneratedDamage = 0.0f;

					// Get random damage from weapon reference
					bool bIsCritical = WeaponRef->GenerateRandomDamageNumber(PlayerRef->GetComboFactor(), GeneratedDamage);

					// Create Damage structure
					FDamageInfo DamageInfo(GeneratedDamage, EDamageType::NormalMeleeSlash, 
						EDamageReactionType::MeleeHitReaction, FCrowdControlInfo(), false, true, true, false);

					bool bHasTakenDamage = 
						Cast<AHYAggroNPCBase>(HitActor)->TakeDamage(DamageInfo, PlayerRef, 0.0f, 0.0f, OutHit);

					if (bHasTakenDamage)
					{
						// Do only when damaged actor is enemy base
						if (AHYEnemyBase* DamagedActor = Cast<AHYEnemyBase>(HitActor))
						{
							// Make Damage Event
							UAISense_Damage::ReportDamageEvent(this, DamagedActor, PlayerRef, GeneratedDamage,
								PlayerRef->GetActorLocation(), FVector::ZeroVector);
							//PlayerRef->AddUltimatePoints(PlayerRef->GetAddedUltimatePoints());

							// If is HP recharge passive activated?
							if (PlayerRef->IsActivatedHPRecharge())
							{
								PlayerRef->Heal(PassiveHealAmount);
							}

							// MP Passive Recharge
							PlayerRef->PassiveMPRecharge(bIsCritical);
						}

					}
				}
			}
		}
	}
}

void UHYWeaponMechanism::ProjectileAimBegin_Implementation()
{
	// Play sound
	UFMODBlueprintStatics::PlayEvent2D(this, BowDrawSound, true);

	bIsAimingProjectile = true;

	PlayerRef->GetCharacterMovement()->bOrientRotationToMovement = false;
	PlayerRef->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	PlayerRef->GetCharacterMovement()->RotationRate = AimRotationRate;

	// Make bow reference valid
	if (nullptr == BowRef)
	{
		BowRef = Cast<AHYBowBase>(WeaponRef);
	}
	
	// Set bow state to be aim and spawn arrow
	if (nullptr != BowRef)
	{
		BowRef->SetBowState(EBowState::Aim);
		SpawnArrow();
		OnAimBegin.Broadcast();
	}
}

void UHYWeaponMechanism::ProjectileAimEnd_Implementation()
{
	// Performs the opposite of Projectile Aim Begin.
	bIsAimingProjectile = false;
	DrawEnd_Implementation();

	PlayerRef->GetCharacterMovement()->bOrientRotationToMovement = true;
	PlayerRef->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	PlayerRef->GetCharacterMovement()->RotationRate = OriginalRotationRate;
	
	// Make bow reference valid
	if (nullptr == BowRef)
	{
		BowRef = Cast<AHYBowBase>(WeaponRef);
	}

	// Set bow state to be idle and destroy arrow
	if (nullptr != BowRef)
	{
		BowRef->SetBowState(EBowState::Idle);
		DestroyArrow();
		OnAimEnd.Broadcast();

		UFMODBlueprintStatics::SetGlobalParameterByName(TEXT("Release"), 1.0f);
	}
}

void UHYWeaponMechanism::FireArrowBegin_Implementation()
{
	// Do only if the bow(or magical things) is drawing.
	if (bIsDrawingBow)
	{
		bIsFiringArrow = true;

		ArrowRef->OnArrowImpact.AddDynamic(this, &UHYWeaponMechanism::DamageActor_Implementation);

		FireAimedArrow();
		DrawEnd_Implementation();

		FTimerHandle FireTimer;
		GetWorld()->GetTimerManager().SetTimer(FireTimer, FTimerDelegate::CreateLambda([this]() {
			this->FireArrowEnd_Implementation();
		}), ShootBowMontage->GetPlayLength(), false);
	}
}

void UHYWeaponMechanism::FireArrowEnd_Implementation()
{
	bIsFiringArrow = false;
	
	// Call Projectile Aim Begin, if the player is still aiming
	if (bIsAimingProjectile)
	{
		ProjectileAimBegin_Implementation();
	}
}

void UHYWeaponMechanism::DrawBegin_Implementation()
{
	if (nullptr == BowRef)
	{
		BowRef = Cast<AHYBowBase>(WeaponRef);
	}

	// Can Draw Bow
	if (!bIsDrawingBow && bIsAimingProjectile && !bIsFiringArrow)
	{
		bIsDrawingBow = true;

		GetWorld()->GetTimerManager().SetTimer(DrawBowTimer, this,
			&UHYWeaponMechanism::IncrementDrawTime_Implementation, DrawIncrementTime, true);
	}
}

void UHYWeaponMechanism::DrawEnd_Implementation()
{
	UFMODBlueprintStatics::SetGlobalParameterByName(TEXT("Release"), 1.0f);
	
	bIsDrawingBow = false;
	GetWorld()->GetTimerManager().ClearTimer(DrawBowTimer);
	DrawTime = 0.0f;
	
	if (nullptr == BowRef)
	{
		BowRef = Cast<AHYBowBase>(WeaponRef);
	}

	BowRef->SetBowState(EBowState::Idle);
}

void UHYWeaponMechanism::IncrementDrawTime_Implementation()
{
	DrawTime = DrawTime + DrawIncrementTime;

	OnDrawOngoing.Broadcast(BowRef->GetMaxDrawTime(), DrawTime);
}

void UHYWeaponMechanism::DamageActor_Implementation(float _Strength, AActor* _HitActor, const FHitResult& _HitResult)
{
	// Set up damage structure.
	float GeneratedDamage = 0.0f;
	_Strength *= 0.0005;
	bool bIsCritical = WeaponRef->GenerateRandomDamageNumber(_Strength, GeneratedDamage);
	FDamageInfo DamageInfo(GeneratedDamage, EDamageType::Projectile, EDamageReactionType::ProjectileHitReaction,
		FCrowdControlInfo(), false, true, true, false);
	
	// Deal damage to the hit actor
	bool bHasTakenDamage = Cast<AHYAggroNPCBase>(_HitActor)->TakeDamage(DamageInfo, PlayerRef, 0.0f, 0.0f, _HitResult);

	if (bHasTakenDamage)
	{
		// Do only when damaged actor is enemy base
		if (AHYEnemyBase* DamagedActor = Cast<AHYEnemyBase>(_HitActor))
		{
			// Make Damage Event
			UAISense_Damage::ReportDamageEvent(this, DamagedActor, PlayerRef, GeneratedDamage,
				_HitResult.Location, FVector::ZeroVector);

			PlayerRef->AddUltimatePoints(PlayerRef->GetAddedUltimatePoints());

			// MP Passive Recharge
			PlayerRef->PassiveMPRecharge(bIsCritical);
		}

	}

}

void UHYWeaponMechanism::ToggleWeaponTrace(bool _WeaponTraceOn)
{
	bWeaponTrace = _WeaponTraceOn;

	// If trace is on, set up socket locations.
	if (bWeaponTrace)
	{
		USkeletalMeshComponent* WeaponMesh = WeaponRef->GetWeaponMesh();

		PreviousTopSocket = WeaponMesh->GetSocketLocation(TEXT("TopTrace"));
		PreviousBottomSocket = WeaponMesh->GetSocketLocation(TEXT("BottomTrace"));
	}
	// If Trace is over, clear all set.
	else
	{
		AlreadyDamaged.Empty();
	}
}
