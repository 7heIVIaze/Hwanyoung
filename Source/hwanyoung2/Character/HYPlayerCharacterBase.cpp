// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYPlayerCharacterBase.h"
#include "Player/HYPlayerCharacController.h"
#include "DataAssets/CharacterStatusType.h"
#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SplineComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Interactables/HYInteractableActor.h"
#include "Inventory/InventoryItem.h"
#include "Interactables/Items/HYAutomaticPickUp.h"
#include "HYMPSystem.h"
#include "Stats/HYAttributeSystem.h"
#include "Stats/HYWeaponMechanism.h"
#include "HYHPSystem.h"
#include "HYAggroNPCBase.h"
#include "DataAssets/CombatTypes.h"
#include "Weapons/HYWeaponBase.h"


// Sets default values for this component's properties
AHYPlayerCharacterBase::AHYPlayerCharacterBase()
{
	// Construct actor components.

	MPSystem = CreateDefaultSubobject<UHYMPSystem>(TEXT("MP System"));
	AttributeSet = CreateDefaultSubobject<UHYAttributeSystem>(TEXT("Attribute Set"));
	WeaponMechanism = CreateDefaultSubobject<UHYWeaponMechanism>(TEXT("WeaponMechanism"));
	HPSystem = CreateDefaultSubobject<UHYHPSystem>(TEXT("HP System"));
	SplinePath = CreateDefaultSubobject<USplineComponent>(TEXT("SplinePath"));

	// For Projectile path tracing
	Sphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	Decal->SetupAttachment(Sphere);
}


// Called when the game starts
void AHYPlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// HP System component validation check
	if (HPSystem)
	{
		// Bind functions to all events of HP system.
		HPSystem->OnDeath.AddDynamic(this, &AHYPlayerCharacterBase::Death);
		HPSystem->OnDamageTaken.AddDynamic(this, &AHYPlayerCharacterBase::DamageResponse);
		HPSystem->OnDamageBlocked.AddDynamic(this, &AHYPlayerCharacterBase::Blocked);
		HPSystem->OnHeal.AddDynamic(this, &AHYPlayerCharacterBase::UpdateHPBar);
		HPSystem->SetMaxAttackTokensCount(1);
	}

	// MP System component validation check
	if (MPSystem)
	{
		// Bind functions to all events of MP system.
		MPSystem->OnManaUsed.AddDynamic(this, &AHYPlayerCharacterBase::UpdateManaBar);
		MPSystem->OnManaReplenished.AddDynamic(this, &AHYPlayerCharacterBase::UpdateManaBar);
		MPSystem->OnUltimatePointsUsed.AddDynamic(this, &AHYPlayerCharacterBase::UpdateUltimatePointsBar);
		MPSystem->OnUltimatePointsAdded.AddDynamic(this, &AHYPlayerCharacterBase::UpdateUltimatePointsBar);
	}

	ControllerRef = Cast<AHYPlayerCharacController>(GetController());

	OriginalMeshMaterial = GetMesh()->GetMaterials();

	CharacterStance = static_cast<int>(ECharacterState::None);

	// Dash timeline binding
	if (nullptr != DashTimelineCurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &AHYPlayerCharacterBase::OnDash);
		DashTimeline.AddInterpFloat(DashTimelineCurveFloat, TimelineProgress);

		FOnTimelineEvent TimelineFinished;
		TimelineFinished.BindDynamic(this, &AHYPlayerCharacterBase::DashFinished);
		DashTimeline.SetTimelineFinishedFunc(TimelineFinished);

		DashTimeline.SetLooping(false);
	}

	// Teleport timeline binding
	if (nullptr != TeleportTimelineCurveVector)
	{
		FOnTimelineVector TimelineProgress;
		TimelineProgress.BindDynamic(this, &AHYPlayerCharacterBase::OnTeleport);
		TeleportTimeline.AddInterpVector(TeleportTimelineCurveVector, TimelineProgress);
	}

	AddPlayerHUDWidgetToViewport();
}


// Called every frame
void AHYPlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Call only if the dash timeline is playing.
	if (DashTimeline.IsPlaying())
	{
		DashTimeline.TickTimeline(DeltaTime);
	}

	if (DeathTimeline.IsPlaying())
	{
		DeathTimeline.TickTimeline(DeltaTime);
	}

	FVector Location = GetActorLocation();
	UKismetMaterialLibrary::SetVectorParameterValue(this, MPC_PlayerLocation, TEXT("PlayerLocation"), FLinearColor(Location.X, Location.Y, Location.Z));
	
	// Current state is climbing
	if (bIsClimbing)
	{
		bool BodyHit, HeadHit;
		FHitResult BodyHitResult, HeadHitResult;
		ClimbingLineTrace(BodyHit, HeadHit, BodyHitResult, HeadHitResult);
		
		if (false == BodyHit)
		{
			StopClimbing();
		}

		if (false == HeadHit)
		{
			bIsClimbing = false;
			
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance && ClimbingLedgeUpMontage)
			{
				AnimInstance->Montage_Play(ClimbingLedgeUpMontage);
			}
		}

		FVector TargetRelativeLocation = BodyHitResult.Location + (GetActorForwardVector() * -1.0f * GetCapsuleComponent()->GetUnscaledCapsuleRadius());
		FRotator TargetRelativeRotation = FRotator(0.0f, UKismetMathLibrary::MakeRotFromX(BodyHitResult.Normal * -1.0f).Yaw, 0.0f);
		UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(), TargetRelativeLocation, TargetRelativeRotation, false, false, 0.2f, false, EMoveComponentAction::Move, FLatentActionInfo());
	}
	// ...
}

void AHYPlayerCharacterBase::OnLanded(const FHitResult& Hit)
{
	Super::OnLanded(Hit);

	// When this character hits the ground, call the fall damage function..
	FallDamageEvent();
}

float AHYPlayerCharacterBase::GetMaxHP()
{
	return HPSystem->GetMaxHP();
}

float AHYPlayerCharacterBase::GetCurrentHP()
{
	return HPSystem->GetCurrentHP();
}

float AHYPlayerCharacterBase::GetMaxMana()
{
	return MPSystem->GetMaxMana();
}

float AHYPlayerCharacterBase::GetCurrentMana()
{
	return MPSystem->GetCurrentMana();
}

float AHYPlayerCharacterBase::GetCurrentUltPoints()
{
	return MPSystem->GetCurrentUltimatePoint();
}

float AHYPlayerCharacterBase::GetMaxUltPoints()
{
	return MPSystem->GetMaxUltimatePoint();
}

void AHYPlayerCharacterBase::QSkillCooldown_Implementation()
{
	// Set the timeline play rate
	QSkillCooldownTimeline.SetPlayRate(1.0f / SkillInfo[ESkillIndex::Q].BaseCooldownTime);

	// Play the cooldown timeline
	QSkillCooldownTimeline.PlayFromStart();
}

void AHYPlayerCharacterBase::QSkillCooldownUpdated(float LeftCooldown)
{
	UKismetMaterialLibrary::SetScalarParameterValue(this, MPC_CircularProgressBar, TEXT("Q Skill Cooldown Progress"), LeftCooldown);
}

void AHYPlayerCharacterBase::QSkillCooldownFinished()
{
	ResetQSkill();

	PlayerHUD->QSkillCooldownPerecnt(SkillInfo[ESkillIndex::Q], 0.0f);
}

void AHYPlayerCharacterBase::ResetQSkill()
{
	SkillInfo[ESkillIndex::Q].bCanBeCast = true;
	SkillInfo[ESkillIndex::Q].bWasCastByPlayer = false;
	// This function will be implemented at each character class
}

void AHYPlayerCharacterBase::ESkillCooldown_Implementation()
{
	// Set the timeline play rate
	ESkillCooldownTimeline.SetPlayRate(1.0f / SkillInfo[ESkillIndex::E].BaseCooldownTime);

	// Play the cooldown timeline
	ESkillCooldownTimeline.PlayFromStart();
}

void AHYPlayerCharacterBase::ESkillCooldownUpdated(float LeftCooldown)
{
	UKismetMaterialLibrary::SetScalarParameterValue(this, MPC_CircularProgressBar, TEXT("E Skill Cooldown Progress"), LeftCooldown);
}

void AHYPlayerCharacterBase::ESkillCooldownFinished()
{
	ResetESkill();

	PlayerHUD->ESkillCooldownPerecnt(SkillInfo[ESkillIndex::E], 0.0f);
}

void AHYPlayerCharacterBase::ResetESkill()
{
	SkillInfo[ESkillIndex::E].bCanBeCast = true;
	SkillInfo[ESkillIndex::E].bWasCastByPlayer = false;
	// This function will be implemented at each character class
}

void AHYPlayerCharacterBase::UseThirdAbility_Implementation()
{
}

void AHYPlayerCharacterBase::CSkillCooldown_Implementation()
{
	// Set the timeline play rate
	CSkillCooldownTimeline.SetPlayRate(1.0f / SkillInfo[ESkillIndex::C].BaseCooldownTime);

	// Play the cooldown timeline
	CSkillCooldownTimeline.PlayFromStart();
}

void AHYPlayerCharacterBase::CSkillCooldownUpdated(float LeftCooldown)
{
	UKismetMaterialLibrary::SetScalarParameterValue(this, MPC_CircularProgressBar, TEXT("C Skill Cooldown Progress"), LeftCooldown);
}

void AHYPlayerCharacterBase::CSkillCooldownFinished()
{
	ResetCSkill();

	PlayerHUD->CSkillCooldownPerecnt(SkillInfo[ESkillIndex::C], 0.0f);
}

void AHYPlayerCharacterBase::ResetCSkill()
{
	SkillInfo[ESkillIndex::C].bCanBeCast = true;
	SkillInfo[ESkillIndex::C].bWasCastByPlayer = false;
	// This function will be implemented at each character class
}

void AHYPlayerCharacterBase::UseUltimateAbility_Implementation()
{
}

void AHYPlayerCharacterBase::UltSkillCooldown_Implementation()
{
	// Set the timeline play rate
	ZSkillCooldownTimeline.SetPlayRate(1.0f / SkillInfo[ESkillIndex::Z].BaseCooldownTime);

	// Play the cooldown timeline
	ZSkillCooldownTimeline.PlayFromStart();
}

void AHYPlayerCharacterBase::UltSkillCooldownUpdated(float LeftCooldown)
{
	UKismetMaterialLibrary::SetScalarParameterValue(this, MPC_CircularProgressBar, TEXT("Z Skill Cooldown Progress"), LeftCooldown);
}

void AHYPlayerCharacterBase::UltSkillCooldownFinished()
{
	ResetUltSkill();

	PlayerHUD->ZSkillCooldownPerecnt(SkillInfo[ESkillIndex::Z], 0.0f);
}

void AHYPlayerCharacterBase::ResetUltSkill()
{
	SkillInfo[ESkillIndex::Z].bCanBeCast = true;
	SkillInfo[ESkillIndex::Z].bWasCastByPlayer = false;
	// This function will be implemented at each character class
}

void AHYPlayerCharacterBase::Blocked_Implementation(bool CanBeParried, AActor* DamageInstigator)
{
	// This function will be implemented at each character class
}

void AHYPlayerCharacterBase::UpdateHPBar()
{
	// Valid check
	if (PlayerHUD)
	{
		PlayerHUD->UpdateHPBar(HPSystem->GetCurrentHP(), HPSystem->GetMaxHP());
	}
}

void AHYPlayerCharacterBase::UpdateManaBar()
{
	// Valid check
	if (PlayerHUD)
	{
		PlayerHUD->UpdateMPBar(MPSystem->GetCurrentMana(), MPSystem->GetMaxMana());
	}
}

void AHYPlayerCharacterBase::UpdateUltimatePointsBar()
{
	// Valid check
	if (PlayerHUD)
	{
		PlayerHUD->UpdateUltimateBar(MPSystem->GetCurrentUltimatePoint(), MPSystem->GetMaxUltimatePoint());
	}
}

void AHYPlayerCharacterBase::UpdateHonbaek(int Amount)
{
	// Valid check
	if (PlayerHUD)
	{
		PlayerHUD->GetSoulNumber(Amount);
	}
}


void AHYPlayerCharacterBase::OnMontageEnded(UAnimMontage* Montage, bool Interrupted)
{
	if (Montage == ClimbingLedgeUpMontage)
	{
		ClimbUpLedge();
	}
}

void AHYPlayerCharacterBase::StartHitStop(float Duration)
{
	CustomTimeDilation = 0.0f;

	GetWorld()->GetTimerManager().SetTimer(HitStopTimer, this, &AHYPlayerCharacterBase::EndHitStop, Duration);
}

void AHYPlayerCharacterBase::EndHitStop()
{
	CustomTimeDilation = 1.0f;
}

void AHYPlayerCharacterBase::UseStamina()
{
	// Use Stamina for climbing(and dash, etc. that's implemented in the future).
	bool bIsSuccessful = MPSystem->UseStamina(0.2f);
	float fPercent = MPSystem->GetCurrentStamina() / MPSystem->GetMaxStamina();

	UKismetMaterialLibrary::SetScalarParameterValue(this, MPC_CircularProgressBar, TEXT("StaminaProgress"), fPercent);

	// If stamina is over, then stop climbing
	if (false == bIsSuccessful)
	{
		if (GetWorldTimerManager().IsTimerActive(StaminaReduceTimer))
		{
			GetWorld()->GetTimerManager().ClearTimer(StaminaReduceTimer);
		}

		StopClimbing();
	}
	
}

void AHYPlayerCharacterBase::ReplenishStamina()
{
	bool bIsReplenishEnded = MPSystem->ReplenishStamina(0.2f);

	float fPercent = MPSystem->GetCurrentStamina() / MPSystem->GetMaxStamina();

	UKismetMaterialLibrary::SetScalarParameterValue(this, MPC_CircularProgressBar, TEXT("StaminaProgress"), fPercent);
	
	if (bIsReplenishEnded)
	{
		if (GetWorldTimerManager().IsTimerActive(StaminaRechargeTimer))
		{
			GetWorld()->GetTimerManager().ClearTimer(StaminaRechargeTimer);
		}
		PlayerHUD->SetStaminaVisibility(false);
	}
}

void AHYPlayerCharacterBase::DeathNofity()
{
	DeathWidget = CreateWidget<UUserWidget>(this, DeathWidgetClass);

	DeathWidget->AddToViewport();

	if (DeathTimelineCurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &AHYPlayerCharacterBase::OnDeathTimelinePlayed);
		DashTimeline.AddInterpFloat(DeathTimelineCurveFloat, TimelineProgress);
	}

	DeathTimeline.PlayFromStart();
}

void AHYPlayerCharacterBase::OnDeathTimelinePlayed(float DeltaTime)
{
	DeathWidget->TextAnimation(DeltaTime);
}

bool AHYPlayerCharacterBase::IsDead()
{
	return HPSystem->GetIsDead();
}

float AHYPlayerCharacterBase::Heal(float AmountToHeal)
{
	return HPSystem->Heal(AmountToHeal);
}

void AHYPlayerCharacterBase::Death_Implementation()
{
	if (IsDead())
	{
		GetMesh()->SetSimulatePhysics(false);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && DeathMontage)
		{
			AnimInstance->Montage_Play(DeathMontage);
		}

		// Stop Character movement and lose a part of current honbaek.
		DisableInput(ControllerRef);
		GetCharacterMovement()->StopMovementImmediately();
		ControllerRef->LoseHonbaek();
	}
}

void AHYPlayerCharacterBase::ReturnAttackToken(int Amount)
{
	HPSystem->ReturnAttackToken(Amount);
}

bool AHYPlayerCharacterBase::ReserveAttackToken(int Amount)
{
	return HPSystem->ReserveAttackToken(Amount);
}

void AHYPlayerCharacterBase::SetSkillInfo(bool CanBeCast, int SkillIndex, int UsedSkillIndex)
{
	// If current skill is used
	if (SkillIndex == UsedSkillIndex)
	{
		SkillInfo[SkillIndex].bCanBeCast = CanBeCast;
		SkillInfo[SkillIndex].bWasCastByPlayer = true;
	}
	else
	{
		SkillInfo[SkillIndex].bCanBeCast = CanBeCast;
		SkillInfo[SkillIndex].bWasCastByPlayer = false;
	}
}

void AHYPlayerCharacterBase::PreventSkillCasting_Implementation(int CastedSkill)
{
	// Prevent player from casting all skills
	for (int i = 0; i < SkillInfo.Num(); ++i)
	{
		SetSkillInfo(false, i, CastedSkill);
	}

	if (nullptr != LockTimelineCurveFloat)
	{
		LockTimeline.AddInterpFloat(LockTimelineCurveFloat, FOnTimelineFloatStatic::CreateLambda([&](float PreventSkillTime)
		{
			PreventCastingSkillTime = PreventSkillTime;
			PlayerHUD->SkillCooldownPercent(SkillInfo, PreventCastingSkillTime);
		}));

		LockTimeline.SetTimelineFinishedFunc(FOnTimelineEventStatic::CreateLambda([&, CastedSkill]() {
			for (int i = 0; i < SkillInfo.Num(); ++i)
			{
				if (i != CastedSkill)
				{
					// 6 is dummy value
					SetSkillInfo(true, i, 6);
					PlayerHUD->SkillCooldownPercent(SkillInfo, 0.0f);
				}
			}
		}));

		LockTimeline.PlayFromStart();
	}

}

void AHYPlayerCharacterBase::Teleport_Implementation(FVector TeleportLocation)
{
	for (int i = 0; i < OriginalMeshMaterial.Num(); ++i)
	{
		DissolveMeshMaterial.Add(GetMesh()->CreateDynamicMaterialInstance(i, TeleportDissolve));
	}

	
	DisableInput(ControllerRef);
	WeaponMechanism->GetWeaponReference()->SetActorHiddenInGame(true);

	// Timeline for animation finish : Teleport.
	TeleportTimeline.SetTimelineFinishedFunc(FOnTimelineEventStatic::CreateLambda([&, TeleportLocation]()
	{
		SetActorLocation(TeleportLocation);
		for (int i = 0; i < OriginalMeshMaterial.Num(); ++i)
		{
			GetMesh()->SetMaterial(i, OriginalMeshMaterial[i]);
			DissolveMeshMaterial[i] = nullptr;
		}

		//this->WeaponMechanism->GetWeaponReference()->SetActorHiddenInGame(false);
		EnableInput(ControllerRef);
	}));

	TeleportTimeline.PlayFromStart();
}

void AHYPlayerCharacterBase::FallDamageEvent_Implementation()
{
	// Perform only if character is falling.
	if (GetCharacterMovement()->IsFalling())
	{
		// Get the velocity, but when falling, the value of z is negative, so negate it for easy calculation.
		FVector vSpeed = GetVelocity();
		vSpeed.Z *= -1.0f;

		// The falling speed after jumping was around 700.0f. Therefore, set to take fall damage when it was over 2000.0f.
		if (2000.0f < vSpeed.Z)
		{
			// Fall damage can be set to a minimum of 20 and a maximum of 500 based on speed.
			FDamageInfo sDamageInfo(FMath::Lerp(20.0f, 500.0f, vSpeed.Z / 20000.0f), 
				EDamageType::Natural, EDamageReactionType::EnvironmentalHitReaction, FCrowdControlInfo(), true, false, false, false);

			// Take Damage
			TakeDamage(sDamageInfo, this, 0.0f, 0.0f, FHitResult());
		}
	}
}

void AHYPlayerCharacterBase::ToggleInventory_Implementation()
{
	if (nullptr == ControllerRef)
	{
		return;
	}

	ControllerRef->ToggleInventoryWidget_Implementation();
	UFMODBlueprintStatics::SetGlobalParameterByName(TEXT("Menu"), 0.0f);

	if (nullptr == MenuOpenSound)
	{
		return;
	}

	UFMODBlueprintStatics::PlayEvent2D(this, MenuOpenSound, true);
}

void AHYPlayerCharacterBase::Dash_Implementation(float DeltaTime)
{
	if (nullptr != DashSound)
	{
		UFMODBlueprintStatics::PlayEvent2D(this, DashSound, true);
	}

	// Can only be used when not jumping or falling(to prevent bunny hops, super jumps, etc.)
	if (false == GetCharacterMovement()->IsFalling())
	{
		if (bCanUseDash)
		{
			GetCharacterMovement()->MaxWalkSpeed = 1500.0f;
			GetCharacterMovement()->MaxWalkSpeed = 15000.0f;
			bCanUseDash = false;

			DashTimeline.PlayFromStart();
		}
	}

	if (nullptr != DashTimelineCurveFloat)
	{
		FTimerHandle DashFinishedTimerHandle;
		float Duration = DashTimelineCurveFloat->FloatCurve.GetLastKey().Time;

		// 
		GetWorld()->GetTimerManager().SetTimer(DashFinishedTimerHandle, FTimerDelegate::CreateLambda([&]() {
			bCanUseDash = true;
		}), Duration + 1.0f, false);

	}
}

bool AHYPlayerCharacterBase::UseUltimatePoints(float ConsumedUltPoints)
{
	return MPSystem->UseUltimatePoints(ConsumedUltPoints);
}

float AHYPlayerCharacterBase::AddUltimatePoints(float AddedUltPoints)
{
	return MPSystem->AddUltimatePoints(AddedUltPoints);
}

float AHYPlayerCharacterBase::ReplenishMana(float AmountOfMana)
{
	return MPSystem->ReplenishMana(AmountOfMana);
}

bool AHYPlayerCharacterBase::UseMana(float ToBeConsumedMana)
{
	return MPSystem->UseMana(ToBeConsumedMana);
}

void AHYPlayerCharacterBase::PassiveMPRecharge(bool IsCriticalDamage)
{
	// if the last damage is critical damage and MP recharge passive is activated
	if (IsCriticalDamage && bIsActivatedMPRecharge)
	{
		MPSystem->ReplenishMana(10.0f);
	}
}

void AHYPlayerCharacterBase::OnDash(float DeltaTime)
{
	// Add Movement to front
	AddMovementInput(GetActorForwardVector(), 1.0f, false);
}

void AHYPlayerCharacterBase::DashFinished()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MaxWalkSpeed = 2048.0f;
}

void AHYPlayerCharacterBase::OnTeleport(FVector Value)
{
	for (auto DissolveMat : DissolveMeshMaterial)
	{
		DissolveMat->SetScalarParameterValue(TEXT("Dissolve"), Value.X);
		DissolveMat->SetScalarParameterValue(TEXT("Strength_VN"), Value.Y);
	}
}

void AHYPlayerCharacterBase::DamageResponse_Implementation(EDamageReactionType DamageResponse, FCrowdControlInfo CrowdControl, 
										AActor* DamageInstigator, const FHitResult& Hit)
{
	// If hit actor is valid and same with this instance
	if (this == Hit.GetActor())
	{
		FVector BoneLocation;
		FName HitBoneName = GetMesh()->FindClosestBone(Hit.ImpactPoint, &BoneLocation);

		// Spawn the blood decal and niagara particles for hit effect
		UGameplayStatics::SpawnDecalAttached(M_BloodGash, DecalSize, GetMesh(), HitBoneName, FVector(0.0f), 
			FRotator::ZeroRotator, EAttachLocation::SnapToTarget, 5.0f);

		UNiagaraComponent* BloodVFX = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, NS_Blood, BoneLocation);
		BloodVFX->Activate();

		PlayerHUD->UpdateHPBar(HPSystem->GetCurrentHP(), HPSystem->GetMaxHP());
	}

	// Timer handle for enabling input.
	FTimerHandle Timer;

	// For damage reaction, distinct the reaction type
	switch (DamageResponse)
	{
		case EDamageReactionType::Stunned:
		{
			// Make input disable for duration.
			DisableInput(ControllerRef);

			GetWorld()->GetTimerManager().SetTimer(Timer, FTimerDelegate::CreateLambda([this]()
			{
				this->EnableInput(this->ControllerRef);
			}), CrowdControl.Duration, false);
		}
		break;
		case EDamageReactionType::KnockedBack:
		{
			// Make input disable for duration.
			DisableInput(ControllerRef);
			
			// Push the character to create a knockback effect.
			LaunchCharacter(CrowdControl.KnockedbackDistance, true, false);

			GetWorld()->GetTimerManager().SetTimer(Timer, FTimerDelegate::CreateLambda([this]()
			{
				this->EnableInput(this->ControllerRef);
			}), CrowdControl.Duration, false);
		}
		break;
		case EDamageReactionType::Airborned:
		{
			// Make input disable for duration.
			DisableInput(ControllerRef);

			// Push the character upward to create a airborne effect
			LaunchCharacter(FVector(0.0f, 0.0f, CrowdControl.AirborneHeight), false, true);

			GetWorld()->GetTimerManager().SetTimer(Timer, FTimerDelegate::CreateLambda([this]()
			{
				this->EnableInput(this->ControllerRef);
			}), CrowdControl.Duration, false);
		}
		break;
		case EDamageReactionType::Feared:
		{
			// Make input disable for duration.
			DisableInput(ControllerRef);

			GetWorld()->GetTimerManager().SetTimer(Timer, FTimerDelegate::CreateLambda([this]()
			{
				this->EnableInput(this->ControllerRef);
			}), CrowdControl.Duration, false);
		}
		break;
		case EDamageReactionType::Rooted:
		{
			// Make movement disable for duration.
			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->DisableMovement();

			GetWorld()->GetTimerManager().SetTimer(Timer, FTimerDelegate::CreateLambda([this]()
			{
				this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
			}), CrowdControl.Duration, false);
		}
		break;
	}
}

bool AHYPlayerCharacterBase::TakeDamage(FDamageInfo DamageInfo, AActor* DamageInstigator,
						float DamageDuration, float DamageInterval, const FHitResult& _Hit)
{
	bool bHasTakenDamage = false;
	
	// Check damage type because some types are DOT damage.
	switch (DamageInfo.DamageType)
	{
		case EDamageType::None:
		case EDamageType::CloseCombat:
		case EDamageType::NormalMeleeSlash:
		case EDamageType::SpiritMeleeSlash:
		case EDamageType::Projectile:
		case EDamageType::Parry:
		case EDamageType::Stun:
		case EDamageType::HealthDrain:
		case EDamageType::ElectricShock:
		{
			bHasTakenDamage = HPSystem->TakeDamage(DamageInfo, DamageInstigator, _Hit);
			UFMODBlueprintStatics::PlayEvent2D(this, PlayerHitSound, true);
		}
		break;
		case EDamageType::Burn:
		case EDamageType::Poison:
		case EDamageType::Bleed:
		case EDamageType::Freeze:
		{
			GetWorld()->GetTimerManager().SetTimer(PlayerDOTDamageTimer, this, &AHYPlayerCharacterBase::StopDOT, DamageDuration, false);
			HPSystem->StartDOT_Implementation(DamageInfo, DamageInstigator, DamageInterval);
		}
		break;
		default:
			break;
	}

	return bHasTakenDamage;
}

void AHYPlayerCharacterBase::StopDOT()
{
	HPSystem->StopDOT_Implementation();
	GetWorld()->GetTimerManager().ClearTimer(PlayerDOTDamageTimer);
}

bool AHYPlayerCharacterBase::ResetAttributes()
{
	return AttributeSet->ResetAttributes();
}

bool AHYPlayerCharacterBase::Buff(FBuffableInfo BuffableInfo)
{
	bool bHasBeenBuffed = false;
	FTimerHandle BuffTimer;
	FVector Location;
	int iVFXIndex;
	// If the buff is applied to status.(there is no CC buffs)
	if (EBuffableCategory::Stat == BuffableInfo.BuffableCategory)
	{
		SetCharacterState(BuffableInfo.BuffableType, true);
		StatStances[BuffableInfo.BuffableType]++;

		// Different effect per buff type
		switch (BuffableInfo.BuffableType)
		{
			// those two buff types affect to AttributeSet actor component
			case ECharacterState::DefenseUp:
			{
				iVFXIndex = 1;
				Location = FVector(0.0f, 0.0f, 0.0f); // the vfx is not prepared yet.
				bHasBeenBuffed = AttributeSet->Buff(BuffableInfo.StatBuffInfo);
				break;
			}
			case ECharacterState::Empower:
			{
				iVFXIndex = 5;
				Location = FVector(0.0f, 0.0f, 0.0f); // the vfx is not prepared yet.
				bHasBeenBuffed = AttributeSet->Buff(BuffableInfo.StatBuffInfo);
				break;
			}
			case ECharacterState::HPBuff:
			{
				iVFXIndex = 7;
				Location = FVector(0.0f, 0.0f, 0.0f); // the vfx is not prepared yet.
				bHasBeenBuffed = HPSystem->Buff(BuffableInfo.Magnitude);
				break;
			}
			case ECharacterState::MPBuff:
			{
				iVFXIndex = 8;
				Location = FVector(0.0f, 0.0f, 0.0f); // the vfx is not prepared yet.
				bHasBeenBuffed = MPSystem->Buff(BuffableInfo.Magnitude);
				break;
			}
		}

		GetWorldTimerManager().SetTimer(BuffTimer, FTimerDelegate::CreateLambda([&, BuffableInfo]()
		{
			// There is no crowdcontrol buffs, so ignore CC case.
			// But exception handling
			if (EBuffableCategory::Stat == BuffableInfo.BuffableCategory)
			{
				// Reduce current buff type numbers
				StatStances[BuffableInfo.BuffableType]--;

				// If this buff type is last one, then remove this type from character stance
				if (StatStances[BuffableInfo.BuffableType] < 0)
				{
					SetCharacterState(BuffableInfo.BuffableType, false);

					// Deactivate the particle
					StancesParticles[BuffableInfo.BuffableType]->Deactivate();
				}

				// And remove this buff
				RemoveBuff(BuffableInfo);
			}
		}), BuffableInfo.Duration, false);

		// If the particle system is in the map, reset system
		if (StancesParticles.Contains(BuffableInfo.BuffableType))
		{
			StancesParticles[BuffableInfo.BuffableType]->ResetSystem();
		}
		// else, spawn system.
		else
		{
			UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
				NS_Buffables[iVFXIndex], GetMesh(), TEXT("None"),
				Location, FRotator(0.0f, 0.0f, 0.0f),
				EAttachLocation::KeepRelativeOffset, true
			);
			StancesParticles[BuffableInfo.BuffableType] = NiagaraComp;
		}

	}

	return bHasBeenBuffed;
}

bool AHYPlayerCharacterBase::Debuff(FBuffableInfo BuffableInfo)
{
	bool bHasBeenBuffed = false;
	FTimerHandle BuffTimer;
	FVector Location;
	int iVFXIndex;

	// If the debuff is applied to status.
	if (EBuffableCategory::Stat == BuffableInfo.BuffableCategory)
	{
		SetCharacterState(BuffableInfo.BuffableType, true);
		StatStances[BuffableInfo.BuffableType]++;

		// Different effect per debuff type
		switch (BuffableInfo.BuffableType)
		{
			// Vulnerable debuff types affect to AttributeSet actor component
			case ECharacterState::Vulnerable:
			{
				iVFXIndex = 1;
				Location = FVector(0.0f, 0.0f, 0.0f); // the vfx is not prepared yet.
				bHasBeenBuffed = AttributeSet->Debuff(BuffableInfo.StatBuffInfo);
				break;
			}
			// slowed debuff affects to character movement component.
			case ECharacterState::Slowed:
			{
				iVFXIndex = 2;
				Location = FVector(0.0f, 0.0f, 0.0f); // the vfx is not prepared yet.
				GetCharacterMovement()->MaxWalkSpeed *= (1 - BuffableInfo.Magnitude);
				bHasBeenBuffed = true;
				break;
			}
			// Weakened debuff types affect to AttributeSet actor component
			case ECharacterState::Weakened:
			{
				iVFXIndex = 6;
				Location = FVector(0.0f, 0.0f, 0.0f); // the vfx is not prepared yet.
				bHasBeenBuffed = AttributeSet->Debuff(BuffableInfo.StatBuffInfo);
				break;
			}
		}

		// For Stat Debuff timer
		GetWorldTimerManager().SetTimer(BuffTimer, FTimerDelegate::CreateLambda([&, BuffableInfo]()
		{
			// Reduce current debuff type numbers
			StatStances[BuffableInfo.BuffableType]--;

			// If this buff type is last one, then remove this type from character stance
			if (StatStances[BuffableInfo.BuffableType] < 0)
			{
				SetCharacterState(BuffableInfo.BuffableType, false);
			}
			// Deactivate the particle
			StancesParticles[BuffableInfo.BuffableType]->Deactivate();

			// And remove this debuff
			RemoveDebuff(BuffableInfo);
		}), BuffableInfo.Duration, false);

		AppliedStatDebuffs.Add(FAppliedDebuffInfo(BuffableInfo, BuffTimer));

		// If the particle system is in the map, reset system
		if (StancesParticles.Contains(BuffableInfo.BuffableType))
		{
			StancesParticles[BuffableInfo.BuffableType]->ResetSystem();
		}
		// else, spawn system.
		else
		{
			UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
				NS_Buffables[iVFXIndex], GetMesh(), TEXT("None"),
				Location, FRotator(0.0f, 0.0f, 0.0f),
				EAttachLocation::KeepRelativeOffset, true);

			StancesParticles[BuffableInfo.BuffableType] = NiagaraComp;
		}

	}
	// If the debuff is crowd control
	else
	{
		SetCharacterState(BuffableInfo.BuffableType, true);

		// If the timer is in map, and it is active, then clear
		if (StanceTimers.Contains(BuffableInfo.BuffableType))
		{
			if (GetWorldTimerManager().IsTimerActive(StanceTimers[BuffableInfo.BuffableType]))
			{
				GetWorldTimerManager().ClearTimer(StanceTimers[BuffableInfo.BuffableType]);
			}
		}

		// Different effect per debuff type
		switch (BuffableInfo.BuffableType)
		{
			// Dizzy effects
			case ECharacterState::Dizzy:
			{
				iVFXIndex = 3;
				Location = FVector(0.0f, 0.0f, 180.0f);
				DisableInput(ControllerRef);
				bHasBeenBuffed = true;
				break;
			}
			case ECharacterState::Stunned:
			{
				iVFXIndex = 4;
				Location = FVector(0.0f, 0.0f, 180.0f);
				DisableInput(ControllerRef);
				bHasBeenBuffed = true;
				break;
			}
			// Silence and blind are not prepared yet.
			case ECharacterState::Silence:
			{
				iVFXIndex = 9;
				Location = FVector(0.0f, 0.0f, 0.0f); // the vfx is not prepared yet.
				bHasBeenBuffed = true;
				break;
			}

			case ECharacterState::Blind:
			{
				iVFXIndex = 10;
				Location = FVector(0.0f, 0.0f, 0.0f); // the vfx is not prepared yet.
				bHasBeenBuffed = true;
				break;
			}
		}

		// For CC Debuff timer
		GetWorldTimerManager().SetTimer(BuffTimer, FTimerDelegate::CreateLambda([&, BuffableInfo]()
		{
			// Remove this type from character stance
			SetCharacterState(BuffableInfo.BuffableType, false);

			// And remove this debuff
			RemoveDebuff(BuffableInfo);

			// Remove the timer handle ref in the map
			StanceTimers.Remove(BuffableInfo.BuffableType);

			// Deactivate the particle
			StancesParticles[BuffableInfo.BuffableType]->Deactivate();

		}), BuffableInfo.Duration, false);

		StanceTimers[BuffableInfo.BuffableType] = BuffTimer;

		// If the particle system is in the map, reset system
		if (StancesParticles.Contains(BuffableInfo.BuffableType))
		{
			StancesParticles[BuffableInfo.BuffableType]->ResetSystem();
		}
		// else, spawn system.
		else
		{
			UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
				NS_Buffables[iVFXIndex], GetMesh(), TEXT("None"),
				Location, FRotator(0.0f, 0.0f, 0.0f), 
				EAttachLocation::KeepRelativeOffset, true);

			StancesParticles[BuffableInfo.BuffableType] = NiagaraComp;
		}
	}

	return bHasBeenBuffed;
}

bool AHYPlayerCharacterBase::RemoveBuff(FBuffableInfo BuffableInfo)
{
	bool bHasRemovedBuff = false;
	// Exception handling
	if (EBuffableCategory::Stat == BuffableInfo.BuffableCategory)
	{
		// remove current affected buff.
		switch (BuffableInfo.BuffableType)
		{
			case ECharacterState::DefenseUp:
			{
				bHasRemovedBuff = AttributeSet->RemoveBuff(BuffableInfo.StatBuffInfo);
				break;
			}
			case ECharacterState::Empower:
			{
				bHasRemovedBuff = AttributeSet->RemoveBuff(BuffableInfo.StatBuffInfo);
				break;
			}
			case ECharacterState::HPBuff:
			{
				bHasRemovedBuff = HPSystem->RemoveBuff(BuffableInfo.Magnitude);
				break;
			}
			case ECharacterState::MPBuff:
			{
				bHasRemovedBuff = MPSystem->RemoveBuff(BuffableInfo.Magnitude);
				break;
			}
		}
	}

	return bHasRemovedBuff;
}

bool AHYPlayerCharacterBase::RemoveDebuff(FBuffableInfo BuffableInfo)
{
	bool bHasRemovedBuff = false;
	// Stat debuff case
	if (EBuffableCategory::Stat == BuffableInfo.BuffableCategory)
	{
		switch (BuffableInfo.BuffableType)
		{
			case ECharacterState::Vulnerable:
			{
				bHasRemovedBuff = AttributeSet->RemoveDebuff(BuffableInfo.StatBuffInfo);
				break;
			}
			case ECharacterState::Weakened:
			{
				bHasRemovedBuff = AttributeSet->RemoveDebuff(BuffableInfo.StatBuffInfo);
				break;
			}
		}
	}
	// CrowdControl debuff case
	else
	{
		switch (BuffableInfo.BuffableType)
		{
			case ECharacterState::Dizzy:
			{
				bHasRemovedBuff = true;
				EnableInput(ControllerRef);
				break;
			}
			case ECharacterState::Stunned:
			{
				bHasRemovedBuff = true;
				EnableInput(ControllerRef);
				break;
			}
			// Silence and blind are not prepared yet.
			case ECharacterState::Silence:
			{
				bHasRemovedBuff = true;
				break;
			}
			case ECharacterState::Blind:
			{
				bHasRemovedBuff = true;
				break;
			}
		}
	}

	return bHasRemovedBuff;
}

bool AHYPlayerCharacterBase::Cleanse()
{
	// Remove Stat Debuffs
	for (auto Debuffs : AppliedStatDebuffs)
	{
		GetWorldTimerManager().ClearTimer(Debuffs.DebuffTimer);
		StatStances[Debuffs.DebuffInfo.BuffableType]--;

		if (StatStances[Debuffs.DebuffInfo.BuffableType] < 0)
		{
			SetCharacterState(Debuffs.DebuffInfo.BuffableType, false);
		}

		RemoveDebuff(Debuffs.DebuffInfo);
	}

	// Remove Crowd Control Debuffs
	TArray<ECharacterState> CCDebuffs;
	StanceTimers.GetKeys(CCDebuffs);
	
	for (int i = CCDebuffs.Num() - 1; i >= 0; --i)
	{
		// Clear debuff timer
		GetWorldTimerManager().ClearTimer(StanceTimers[CCDebuffs[i]]);

		// Removing crowd controls doesn't need the numerical value.
		// it's just need buffabletype.
		RemoveDebuff(FBuffableInfo(EBuffableCategory::CrowdControl, CCDebuffs[i], 0.0f,
		FCrowdControlInfo(), FAttributeSetInfo(), 0.0f));

		StancesParticles[CCDebuffs[i]]->Deactivate();
		StancesParticles.Remove(CCDebuffs[i]);
		SetCharacterState(CCDebuffs[i], false);

		StanceTimers.Remove(CCDebuffs[i]);
	}

	return false;
}

void AHYPlayerCharacterBase::Interact_Implementation()
{
	if (nullptr != ControllerRef)
	{
		ControllerRef->Interact();
	}
}

void AHYPlayerCharacterBase::AddPlayerHUDWidgetToViewport_Implementation()
{
	// Create widget and initialize all variables
	PlayerHUD = CreateWidget<UPlayerHUDWidget>(this, PlayerHUDWidget);
	PlayerHUD->DamagableActor = this;
	PlayerHUD->ManaUser = this;
	PlayerHUD->ControllerRef = ControllerRef;

	//// Add widget to viewport
	PlayerHUD->AddToViewport();

	PlayerCrosshair = CreateWidget<UPlayerCrossHair>(this, PlayerCrosshairWidget);
	PlayerCrosshair->AddToViewport();
}

void AHYPlayerCharacterBase::RemovePlayerHUDWidget_Implementation()
{
	PlayerHUD->RemoveFromParent();
	PlayerCrosshair->RemoveFromParent();
}

void AHYPlayerCharacterBase::ParrySuccess_Implementation(AActor* DamageInstigator)
{
	PreventSkillCasting(4);
	/*if (AHYAggroNPCBase* AggroNPC = Cast<AHYAggroNPCBase>(DamageInstigator))
	{
		AggroNPC->Stun();
	}*/
}

void AHYPlayerCharacterBase::ParryFailed_Implementation(AActor* DamageInstigator)
{

}
