// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYHaesol.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineComponent.h"
#include "Components/DecalComponent.h"
#include "Perception/AISense_Damage.h"
#include "HYMPSystem.h"
#include "HYHPSystem.h"
#include "Stats/HYWeaponMechanism.h"
#include "HYAggroNPCBase.h"
#include "HYPoolSubSystem.h"
//#include "HYAOEHeavenlyArrow.h"

AHYHaesol::AHYHaesol()
{

}

void AHYHaesol::BeginPlay()
{
	Super::BeginPlay();

	OriginalFOV = GetFollowCamera()->FieldOfView;
	OriginalBoomOffset = GetCameraBoom()->SocketOffset;
	
	// Initialize the projectiles in the object pool.
	UHYPoolSubSystem* PoolSystem = Cast<UHYPoolSubSystem>(GetGameInstance());
	PoolSystem->InitializePool(ProjectileClass, 10);
}

void AHYHaesol::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#pragma region Deprecated
	/*
	// Update E Skill cooldown progress bar for HUD
	if (true == GetWorldTimerManager().IsTimerActive(ESkillCooldownTimer))
	{
		float fCooldownTime = GetWorldTimerManager().GetTimerElapsed(ESkillCooldownTimer) / SkillInfo[0].BaseCooldownTime;
		UKismetMaterialLibrary::SetScalarParameterValue(this, MPC_CircularProgressBar, TEXT("E Skill Cooldown Progress"), fCooldownTime);
	}
	else
	{
		UKismetMaterialLibrary::SetScalarParameterValue(this, MPC_CircularProgressBar, TEXT("E Skill Cooldown Progress"), 1.0f);
	}

	// Update Q Skill cooldown progress bar for HUD
	if (true == GetWorldTimerManager().IsTimerActive(QSkillCooldownTimer))
	{
		float fCooldownTime = GetWorldTimerManager().GetTimerElapsed(QSkillCooldownTimer) / SkillInfo[1].BaseCooldownTime;
		UKismetMaterialLibrary::SetScalarParameterValue(this, MPC_CircularProgressBar, TEXT("Q Skill Cooldown Progress"), fCooldownTime);
	}
	else
	{
		UKismetMaterialLibrary::SetScalarParameterValue(this, MPC_CircularProgressBar, TEXT("Q Skill Cooldown Progress"), 1.0f);
	}

	// Update C Skill cooldown progress bar for HUD
	if (true == GetWorldTimerManager().IsTimerActive(CSkillCooldownTimer))
	{
		float fCooldownTime = GetWorldTimerManager().GetTimerElapsed(CSkillCooldownTimer) / SkillInfo[2].BaseCooldownTime;
		UKismetMaterialLibrary::SetScalarParameterValue(this, MPC_CircularProgressBar, TEXT("C Skill Cooldown Progress"), fCooldownTime);
	}
	else
	{
		UKismetMaterialLibrary::SetScalarParameterValue(this, MPC_CircularProgressBar, TEXT("C Skill Cooldown Progress"), 1.0f);
	}

	// Update Ult Skill cooldown progress bar for HUD
	if (true == GetWorldTimerManager().IsTimerActive(UltSkillCooldownTimer))
	{
		float fCooldownTime = GetWorldTimerManager().GetTimerElapsed(UltSkillCooldownTimer) / SkillInfo[3].BaseCooldownTime;
		UKismetMaterialLibrary::SetScalarParameterValue(this, MPC_CircularProgressBar, TEXT("Z Skill Cooldown Progress"), fCooldownTime);
	}
	else
	{
		UKismetMaterialLibrary::SetScalarParameterValue(this, MPC_CircularProgressBar, TEXT("Z Skill Cooldown Progress"), 1.0f);
	}
	*/
#pragma endregion

	if (true == WeaponMechanism->GetIsAimingProjectile())
	{
		// Gate 
		if (bIsShowProjPathOpened)
		{
			//ClearProjectileTrajectory();

			// Set up the variables for Predict Projectile Path.
			FVector StartPos = GetMesh()->GetSocketLocation(TEXT("Head_MSocket"));
			FVector CameraForward = GetFollowCamera()->GetForwardVector();
			FVector LaunchVelocity = (CameraForward * ProjAOEDirection + CameraForward) * 1000.0f;
			FHitResult OutHit;
			TArray<FVector>OutPathLocations;
			FVector OutLastTraceDestination;

			UGameplayStatics::Blueprint_PredictProjectilePath_ByTraceChannel(this, OutHit, OutPathLocations, OutLastTraceDestination, StartPos, LaunchVelocity, true, 2.0f, ProjectilePathTraceChannel, false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, 0.0f, 20.0f, 2.0f, 0.0f);

			for (int i = 0; i < OutPathLocations.Num(); ++i)
			{
				SplinePath->AddSplinePointAtIndex(OutPathLocations[i], i, ESplineCoordinateSpace::World);
			}

			SplinePath->SetSplinePointType(OutPathLocations.Num() - 1, ESplinePointType::CurveClamped);
			int SplinePointsNum = SplinePath->GetNumberOfSplinePoints() - 4;

			for (int i = 0; i < SplinePointsNum; ++i)
			{
				//AddComponent();
			}

		}
	}
}

// This is for haesol's third ability(C Skill)
void AHYHaesol::UseThirdAbility_Implementation()
{
	// Only works when this skill can be cast.
	if (true == SkillInfo[ESkillIndex::C].bCanBeCast)
	{
		// If the player is casting ultimate skill, this skill cannot be cast.
		/*if (true == bIsCastingUltSkill)
		{
			return;
		}*/

		//// If the player is already casting C skill, stop this logic.
		//if (true == bIsCastingCSkill)
		//{
		//	return;
		//}

		// If the current weapon is not projectile, then change it.
		if (WeaponIndex != EWeaponIndex::Projectile)
		{
			// Change weapon to projectile.
			//ChangeAttackToHitScan();
		}

		SkillInfo[ESkillIndex::C].bCanBeCast = false;

		// Use Mana
		bool bIsSuccessful = MPSystem->UseMana(40.0f);

		// If failed to use mana, stop this logic
		if (false == bIsSuccessful)
		{
			// Call parent's reset c skill
			//ResetCSkill();
		}

		// Change visibility for projectile trace vfx
		ProjectilePathTraceChannel = ECollisionChannel::ECC_Visibility;

		// Set decal for C skill path
		Decal->SetDecalMaterial(M_HaesolProjectilePathAOE);

		Sphere->SetRelativeScale3D(FVector(3.1f));
		Sphere->SetVisibility(true);
		
		//ShowProjPath();

		GetWorld()->GetTimerManager().SetTimer(ESkillCastingTimer, this, &AHYHaesol::CSkillCooldown, 5.0f, true);
	}
}

void AHYHaesol::CSkillCooldown_Implementation()
{
	// Set invisible the trace path
	Sphere->SetVisibility(false);
	//HideProjPath();

	SkillInfo[ESkillIndex::C].bCanBeCast = true;
	SkillInfo[ESkillIndex::C].bWasCastByPlayer = false;
	
	Super::CSkillCooldown_Implementation();
}

void AHYHaesol::UseUltimateAbility_Implementation()
{
	// Only works when this skill can be cast.
	if (true == SkillInfo[ESkillIndex::Z].bCanBeCast)
	{
		// If the player is casting C skill, this skill cannot be cast.
		/*if (true == bIsCastingCSkill)
		{
			return;
		}*/

		//// If the player is already casting ultimate skill, stop this logic.
		//if (true == bIsCastingUltSkill)
		//{
		//	return;
		//}

		// If the current weapon is not projectile, then change it.
		if (WeaponIndex != EWeaponIndex::Projectile)
		{
			// Change weapon to projectile.
			//ChangeAttackToHitScan();
		}

		//bIsCastingUltSkill = true;
		SkillInfo[ESkillIndex::Z].bCanBeCast = false;

		// Use ultimate points
		bool bIsSuccessful = MPSystem->UseUltimatePoints(60.0f);

		// insufficient points to use
		if (false == bIsSuccessful)
		{
			//ResetUltSkill();
		}

		// If inactivate Grit, then consume HP
		if (HPSystem->GetCurrentHP() >= 35.0f)
		{
			HPSystem->UseHP(30.0f);
		}

		// Preventing another skill casting
		PreventSkillCasting(ESkillIndex::Z);

		// Disable moving while casting
		GetCharacterMovement()->DisableMovement();

		// Show the projectile path
		ProjectilePathTraceChannel = ECollisionChannel::ECC_GameTraceChannel1;
		Decal->SetDecalMaterial(M_HaesolUltPathAOE);
		Sphere->SetRelativeScale3D(FVector(5.1f));
		Sphere->SetVisibility(true);
		//ShowProjPath();

		// the time to determine where to use is in 5 sceonds.
		GetWorld()->GetTimerManager().SetTimer(UltSkillCastingTimer, this, &AHYHaesol::UltSkillCooldown, 5.0f);
	}
}

void AHYHaesol::UltSkillCooldown_Implementation()
{
	// Set invisible the trace path
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	Sphere->SetVisibility(false);
	//HideProjPath();

	SkillInfo[ESkillIndex::Z].bCanBeCast = true;
	SkillInfo[ESkillIndex::Z].bWasCastByPlayer = true;

	Super::UltSkillCooldown_Implementation();
}

void AHYHaesol::ResetQSkill()
{
	Super::ResetQSkill();
}

void AHYHaesol::ResetCSkill()
{
	Super::ResetCSkill();
}

void AHYHaesol::ResetESkill()
{
	Super::ResetESkill();
}

void AHYHaesol::ResetUltSkill()
{
	Super::ResetUltSkill();
}

void AHYHaesol::ActivateThirdSkill_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(ESkillCastingTimer);

	CSkillCooldown();

	GetWorld()->SpawnActor<AActor>(HeavenlyArrowClass, Sphere->GetComponentTransform(), FActorSpawnParameters::FActorSpawnParameters());

	Sphere->SetRelativeLocation(FVector(0.0f));

	AddUltimatePoints(3.0f);
}

void AHYHaesol::ActivateUltSkill_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(UltSkillCastingTimer);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	UltSkillCooldown();
	
	// Set up the parameter values for spawning ult AOE field
	FVector SpawnLocation = Sphere->GetComponentLocation();
	FTransform SpawnTransform = Sphere->GetComponentTransform();

	SpawnLocation.Z += 20.0f;

	GetWorld()->SpawnActor<AActor>(HaesolUltClass, FTransform(SpawnTransform.Rotator(), SpawnLocation, FVector(1.0f)),
		FActorSpawnParameters::FActorSpawnParameters());

	Sphere->SetRelativeLocation(FVector(0.0f));
}

void AHYHaesol::Blocked_Implementation(bool _CanBeParried, AActor* _DamageInstigator)
{
	BlockingState = 0;

	// If the damage instigator is a part of aggro npc
	if (AHYAggroNPCBase* AggroNPC = Cast<AHYAggroNPCBase>(_DamageInstigator))
	{
		// If the aggro NPC's status is can be parried, and the attack, called this logic, is able to be parried	
		// And the player's current status is can be parrying
		if (AggroNPC->bCanBeParried && _CanBeParried && bIsParrying)
		{
			PreventSkillCasting(ESkillIndex::ALL);

			// Play Aggro NPC's parried motion
			AggroNPC->Parried();

			// Make slowed for dramatic effect
			UGameplayStatics::SetGlobalTimeDilation(this, 0.3f);

			ParrySuccess(_DamageInstigator);
		}
		else
		{
			ParryFailed(_DamageInstigator);
		}
	}

}

void AHYHaesol::ParrySuccess_Implementation(AActor* DamageInstigator)
{
	// This function will be called in ParrySuccess blueprint event using 'Call Parent Function'.
	// And it will be called at On Notify Begin of the Play Montage Node.
	Super::ParrySuccess_Implementation(DamageInstigator);
	
	FDamageInfo sDamageInfo(100.0f, EDamageType::Parry, EDamageReactionType::Stagger, FCrowdControlInfo::FCrowdControlInfo(), true, false, false, true);
	AHYAggroNPCBase* ParriedNPC = Cast<AHYAggroNPCBase>(DamageInstigator);

	bool bHasDamageTaken = ParriedNPC->TakeDamage(sDamageInfo, this, 0.0f, 0.0f, FHitResult());

	if (true == bHasDamageTaken)
	{
		UAISense_Damage::ReportDamageEvent(this, DamageInstigator, this, sDamageInfo.DamageAmount, GetActorLocation(), FVector(0.0f));

		AddUltimatePoints(3.0f);
	}
	else
	{
		//EndBlocking();
	}
}

void AHYHaesol::ParryFailed_Implementation(AActor* DamageInstigator)
{
	//EndBlocking();
}


