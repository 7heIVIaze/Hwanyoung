// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYAggroNPCBase.h"

#include "DataAssets/CharacterStatusType.h"
#include "DataAssets/CombatTypes.h"
#include "HYPlayerCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"
#include "AIPlayer/HYAIController.h"
#include "HYHPSystem.h"
#include "HYGameInstance.h"
#include "Stats/HYCombatSystem.h"
#include "Stats/HYAttributeSystem.h"
#include "DialogueableSystem/HYDialogueComponent.h"
#include "Kismet/GameplayStatics.h"
#include "System/HYGroupManagerSubsystem.h"
#include "UI/HYHealthBar.h"
#include "UI/HYDamageAmount.h"
#include "System/HYPoolSubSystem.h"
#include "Stats/HYAttributeSystem.h"

#pragma region Teammates Did(Already Implemented)
AHYAggroNPCBase::AHYAggroNPCBase()
{
	CombatSystem = CreateDefaultSubobject<UHYCombatSystem>(TEXT("Combat System"));
	HPSystem = CreateDefaultSubobject<UHYHPSystem>(TEXT("HP System"));
	DialogueSystem = CreateDefaultSubobject<UHYDialogueComponent>(TEXT("Dialogue System"));

	HitVFXPoint = CreateDefaultSubobject<USceneComponent>(TEXT("HitVFXPoint"));
	HitVFXPoint->SetupAttachment(RootComponent);

	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	Widget->SetupAttachment(RootComponent);

	AttributeSet = CreateDefaultSubobject<UHYAttributeSystem>(TEXT("AttributeSet"));
}
#pragma endregion

void AHYAggroNPCBase::BeginPlay()
{
	Super::BeginPlay();

	GetLoreDataFromInstance();
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->OnMontageEnded.AddDynamic(this, &AHYAggroNPCBase::OnMontageEnded);

#pragma region Teammates Did(Already Implemented)
	// Set up health bar widget.
	UHYHealthBar* NPCHealthBarHUD = CreateWidget<UHYHealthBar>(this, NPCHealthWidget);
	NPCHealthBarHUD->SetDamagableActor(this);
	Widget->SetWidget(NPCHealthBarHUD);
	NPCHealthBarHUD->SetPercentage();
	Widget->SetVisibility(false);

	// Set up controller instance and current state
	AICBase = Cast<AHYAIController>(GetController());
	SetCharacterState(ECharacterState::None, true);

	// Set up binding to events
	HPSystem->OnDeath.AddDynamic(this, &AHYAggroNPCBase::Death);
	HPSystem->OnDamageTaken.AddDynamic(this, &AHYAggroNPCBase::DamageTaken);

	//DialogueSystem->OnDialogueFinished.AddDynamic(this, &AHYAggroNPCBase::EndDialogue);
	CombatSystem->OnTraceDamage.AddDynamic(this, &AHYAggroNPCBase::DealDamage);

	if (DissolveCurve)
	{
		FOnTimelineFloat DissolveProgress;
		DissolveProgress.BindDynamic(this, &AHYAggroNPCBase::OnDissolvePlayed);
		DissolveTimeline.AddInterpFloat(DissolveCurve, DissolveProgress);

		FOnTimelineEvent DissolveFinished;
		DissolveFinished.BindDynamic(this, &AHYAggroNPCBase::OnDissolveFinished);
		DissolveTimeline.SetTimelineFinishedFunc(DissolveFinished);
	}
#pragma endregion

	UHYGroupManagerSubsystem* GroupManagerSubsystem = GetGameInstance()->GetSubsystem<UHYGroupManagerSubsystem>();
	if (GroupManagerSubsystem)
	{
		GroupManagerSubsystem->RegisterGroup(this, GroupId);
	}
}

void AHYAggroNPCBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DissolveTimeline.IsPlaying())
	{
		DissolveTimeline.TickTimeline(DeltaTime);
	}
}

void AHYAggroNPCBase::Death(AActor* DamageInstigator)
{
	// Do when npc is dead
	if (IsDead())
	{
		// Spirituality Increased.(like XP)
		auto AttributeSystem = DamageInstigator->GetComponentByClass<UHYAttributeSystem>();
		AttributeSystem->SpiritualityChange(TEXT("MonsterDeath"));

#pragma region Teammates Did(Already Implemented)
		// Stop behavior
		AICBase->GetBrainComponent()->StopLogic(TEXT(""));

		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
#pragma endregion

		// Remove all attached arrows
		TArray<AActor*> AttachedActors;

		GetAttachedActors(AttachedActors, true, false);
		for (auto AttachedActor : AttachedActors)
		{
			// Return the arrow actor to the pool
			GetGameInstance()->GetSubsystem<UHYPoolSubSystem>()->ReturnToPool(AttachedActor);
		}

		TArray<AActor*> ActorsToReturnTokens;
		ReservedAttackTokens.GetKeys(ActorsToReturnTokens);

		// Return current taken tokens
		for (auto ActorAndToken : ReservedAttackTokens)
		{
			Cast<AHYPlayerCharacterBase>(ActorAndToken.Key)->ReturnAttackToken(ActorAndToken.Value);
		}
		
#pragma region Teammates Did(Already Implemented)
		UnequipWeapon();
		DeathDissolve();
#pragma endregion
	}
}

void AHYAggroNPCBase::Parried()
{
	//AICBase->SetStateAsFrozen();
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	// Remove current attack target
	AICBase->SetStateAsFrozen();


	if (AnimInstance && StaggerMontage)
	{
		// Not used
		/*FOnMontageEnded EndDelegate;
		EndDelegate.BindLambda([this](UAnimMontage* AnimMontage, bool IsEnded)
		{
			AICBase->SetStateAsAttacking(AICBase->AttackTarget, false);
		});
		AnimInstance->Montage_SetEndDelegate(EndDelegate);*/
		

		AnimInstance->Montage_Play(StaggerMontage);
		//PlayAnimMontage(StaggerMontage);
	}
}

void AHYAggroNPCBase::GetLoreDataFromInstance()
{
	UHYGameInstance* GameInstance = Cast<UHYGameInstance>(GetGameInstance());
	auto LoreData = GameInstance->GetLoreData();

	if (auto LoreValue = LoreData.Find(BossNPCRowName))
	{
		if (LoreValue->bHasBeenSlain)
		{
			Destroy();
		}
	}
}

void AHYAggroNPCBase::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == StaggerMontage)
	{
		AttackEnd(AICBase->AttackTarget);
		AICBase->SetStateAsAttacking(AICBase->AttackTarget, false);
	}
}

void AHYAggroNPCBase::InitAggroNPCState()
{
	FSaveData SaveData = GetGameInstance<UHYGameInstance>()->SaveData;
	if (FLore* LoreData = SaveData.LoreData.Find(BossNPCRowName))
	{
		if (LoreData->bHasBeenSlain)
		{
			Destroy();
		}
	}
}

void AHYAggroNPCBase::EndDialogue(AHYPlayerCharacterBase* PlayerRef)
{
	OnDialogueEnd.Broadcast();
}

void AHYAggroNPCBase::StartDialogue(AHYPlayerCharacterBase* PlayerRef)
{
	DialogueSystem->StartDialogue(PlayerRef);
}

void AHYAggroNPCBase::UpdateHPBar()
{
	UHYHealthBar* NPCHealthBarWidget = Cast<UHYHealthBar>(Widget->GetUserWidgetObject());

	NPCHealthBarWidget->SetPercentage();
}


void AHYAggroNPCBase::NotifyGroupAttacked(AActor* AttackTarget)
{
	AICBase->NoticeEnemy(AttackTarget);
}

void AHYAggroNPCBase::ToggleParryTiming(bool IsParryable)
{
	bCanBeParried = IsParryable;
}

bool AHYAggroNPCBase::Buff(FBuffableInfo BuffInfo)
{
	bool bHasBeenBuffed = false;
	FVector Location;
	int VFXIndex;

	// Only stat buff
	if (BuffInfo.BuffableCategory == EBuffableCategory::Stat)
	{
		SetCharacterState(BuffInfo.BuffableType, true);

		StatStance.FindOrAdd(BuffInfo.BuffableType)++;

		// This is for buff. So ignore the debuff types
		switch (BuffInfo.BuffableType)
		{
			case ECharacterState::DefenseUp:
				bHasBeenBuffed = AttributeSet->Buff(BuffInfo.StatBuffInfo);
				break;
			case ECharacterState::Empower:
				bHasBeenBuffed = AttributeSet->Buff(BuffInfo.StatBuffInfo);
				break;
			case ECharacterState::HPBuff:
				bHasBeenBuffed = HPSystem->Buff(BuffInfo.Magnitude);
				break;
				// None is not buff type, so just return.
			case ECharacterState::None:
				return;
		}

		// The BuffableType enum is designed to be used as bit flags, so the index for VFX can be calculated by finding the position of the single set bit in the BuffableType value.
		VFXIndex = FMath::FloorLog2(static_cast<int32>(BuffInfo.BuffableType));

		FTimerHandle BuffTimer;
		GetWorldTimerManager().SetTimer(BuffTimer, FTimerDelegate::CreateLambda([this, BuffInfo]()
		{
			// There is no crowdcontrol buffs, so ignore CC case.
			// But exception handling
			if (EBuffableCategory::Stat == BuffInfo.BuffableCategory)
			{
				// Reduce current buff type numbers
				StatStance[BuffInfo.BuffableType]--;

				// If this buff type is last one, then remove this type from character stance
				if (StatStance[BuffInfo.BuffableType] < 0)
				{
					SetCharacterState(BuffInfo.BuffableType, false);

					// Deactivate the particle
					StanceParticles[BuffInfo.BuffableType]->Deactivate();
				}

				// And remove this buff
				RemoveBuff(BuffInfo);
			}
		}), BuffInfo.Duration, false);

		// If the particle system is in the map, reset system
		if (StanceParticles.Contains(BuffInfo.BuffableType))
		{
			StanceParticles[BuffInfo.BuffableType]->ResetSystem();
		}
		// else, spawn system.
		else
		{
			UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
				NS_Buffables[VFXIndex], GetMesh(), TEXT("None"),
				Location, FRotator(0.0f, 0.0f, 0.0f),
				EAttachLocation::KeepRelativeOffset, true);

			StanceParticles[BuffInfo.BuffableType] = NiagaraComp;
		}
	}

	return bHasBeenBuffed;
}

bool AHYAggroNPCBase::RemoveBuff(FBuffableInfo BuffInfo)
{
	bool bHasRemovedBuff = false;
	if(BuffInfo.BuffableCategory == EBuffableCategory::Stat)
	{
		switch (BuffInfo.BuffableType)
		{
			case ECharacterState::DefenseUp:
				bHasRemovedBuff = AttributeSet->RemoveBuff(BuffInfo.StatBuffInfo);
				break;
			case ECharacterState::Empower:
				bHasRemovedBuff = AttributeSet->RemoveBuff(BuffInfo.StatBuffInfo);
				break;
			case ECharacterState::HPBuff:
				bHasRemovedBuff = HPSystem->RemoveBuff(BuffInfo.Magnitude);
				break;
			// None is not buff type, so just return.
			case ECharacterState::None:
				return;
		}
	}

	return bHasRemovedBuff;
}

bool AHYAggroNPCBase::Debuff(FBuffableInfo BuffInfo)
{
	bool bHasBeenBuffed = false;
	FTimerHandle BuffTimer;
	FVector Location;
	int VFXIndex;

	// If the debuff is applied to status.
	if (EBuffableCategory::Stat == BuffInfo.BuffableCategory)
	{
		SetCharacterState(BuffInfo.BuffableType, true);
		StatStance[BuffInfo.BuffableType]++;

		// Different effect per debuff type
		switch (BuffInfo.BuffableType)
		{
			// Vulnerable debuff types affect to AttributeSet actor component
			case ECharacterState::Vulnerable:
				Location = FVector(0.0f, 0.0f, 0.0f); // the vfx is not prepared yet.
				bHasBeenBuffed = AttributeSet->Debuff(BuffInfo.StatBuffInfo);
				break;
			// slowed debuff affects to character movement component.
			case ECharacterState::Slowed:
				Location = FVector(0.0f, 0.0f, 0.0f); // the vfx is not prepared yet.
				GetCharacterMovement()->MaxWalkSpeed *= (1 - BuffInfo.Magnitude);
				bHasBeenBuffed = true;
				break;
			// Weakened debuff types affect to AttributeSet actor component
			case ECharacterState::Weakened:
				Location = FVector(0.0f, 0.0f, 0.0f); // the vfx is not prepared yet.
				bHasBeenBuffed = AttributeSet->Debuff(BuffInfo.StatBuffInfo);
				break;
			// None is not buff type, so just return.
			case ECharacterState::None:
				return;
		}

		// The BuffableType enum is designed to be used as bit flags, so the index for VFX can be calculated by finding the position of the single set bit in the BuffableType value.
		VFXIndex = FMath::FloorLog2(static_cast<int32>(BuffInfo.BuffableType));

		// For Stat Debuff timer
		GetWorldTimerManager().SetTimer(BuffTimer, FTimerDelegate::CreateLambda([this, BuffInfo]()
		{
			// Reduce current debuff type numbers
			StatStance[BuffInfo.BuffableType]--;

			// If this buff type is last one, then remove this type from character stance
			if (StatStance[BuffInfo.BuffableType] < 0)
			{
				SetCharacterState(BuffInfo.BuffableType, false);
			}
			// Deactivate the particle
			StanceParticles[BuffInfo.BuffableType]->Deactivate();

			// And remove this debuff
			RemoveDebuff(BuffInfo);
		}), BuffInfo.Duration, false);

		AppliedStatDebuffs.Add(FAppliedDebuffInfo(BuffInfo, BuffTimer));

		// If the particle system is in the map, reset system
		if (StanceParticles.Contains(BuffInfo.BuffableType))
		{
			StanceParticles[BuffInfo.BuffableType]->ResetSystem();
		}
		// else, spawn system.
		else
		{
			UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
				NS_Buffables[VFXIndex], GetMesh(), TEXT("None"),
				Location, FRotator(0.0f, 0.0f, 0.0f),
				EAttachLocation::KeepRelativeOffset, true);

			StanceParticles[BuffInfo.BuffableType] = NiagaraComp;
		}

	}
	// If the debuff is crowd control
	else
	{
		SetCharacterState(BuffInfo.BuffableType, true);

		// If the timer is in map, and it is active, then clear
		if (StanceTimer.Contains(BuffInfo.BuffableType))
		{
			if (GetWorldTimerManager().IsTimerActive(StanceTimer[BuffInfo.BuffableType]))
			{
				GetWorldTimerManager().ClearTimer(StanceTimer[BuffInfo.BuffableType]);
			}
		}

		// Different effect per debuff type
		switch (BuffInfo.BuffableType)
		{
			// Dizzy effects
			case ECharacterState::Dizzy:
				Location = FVector(0.0f, 0.0f, 180.0f);
				AICBase->SetStateAsFrozen();
				bHasBeenBuffed = true;
				break;
			case ECharacterState::Stunned:
				Location = FVector(0.0f, 0.0f, 180.0f);
				AICBase->SetStateAsFrozen();
				bHasBeenBuffed = true;
				break;
			// Silence and blind are not prepared yet.
			case ECharacterState::Silence:
				Location = FVector(0.0f, 0.0f, 0.0f); // the vfx is not prepared yet.
				// Silence effect is not implemented yet, but set character state for future use.
				bHasBeenBuffed = true;
				break;

			case ECharacterState::Blind:
				Location = FVector(0.0f, 0.0f, 0.0f); // the vfx is not prepared yet.
				// Silence effect is not implemented yet, but set character state for future use.
				bHasBeenBuffed = true;
				break;
				// None is not buff type, so just return.
			case ECharacterState::None:
				return;
		}

		// The BuffableType enum is designed to be used as bit flags, so the index for VFX can be calculated by finding the position of the single set bit in the BuffableType value.
		VFXIndex = FMath::FloorLog2(static_cast<int32>(BuffInfo.BuffableType));

		// For CC Debuff timer
		GetWorldTimerManager().SetTimer(BuffTimer, FTimerDelegate::CreateLambda([this, BuffInfo]()
		{
			// Remove this type from character stance
			SetCharacterState(BuffInfo.BuffableType, false);

			// And remove this debuff
			RemoveDebuff(BuffInfo);

			// Remove the timer handle ref in the map
			StanceTimer.Remove(BuffInfo.BuffableType);

			// Deactivate the particle
			StanceParticles[BuffInfo.BuffableType]->Deactivate();

		}), BuffInfo.Duration, false);

		StanceTimer[BuffInfo.BuffableType] = BuffTimer;

		// If the particle system is in the map, reset system
		if (StanceParticles.Contains(BuffInfo.BuffableType))
		{
			StanceParticles[BuffInfo.BuffableType]->ResetSystem();
		}
		// else, spawn system.
		else
		{
			UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
				NS_Buffables[VFXIndex], GetMesh(), TEXT("None"),
				Location, FRotator(0.0f, 0.0f, 0.0f),
				EAttachLocation::KeepRelativeOffset, true);

			StanceParticles[BuffInfo.BuffableType] = NiagaraComp;
		}
	}

	return bHasBeenBuffed;
}

bool AHYAggroNPCBase::RemoveDebuff(FBuffableInfo BuffInfo)
{
	return false;
}

void AHYAggroNPCBase::DamageTaken(EDamageReactionType DamageResponse, FCrowdControlInfo CrowdControlInfo,
	AActor* DamageInstigator, const FHitResult& Hit)
{
	AActor* HitActor = Hit.GetActor();

	// Notify group members when attacked
	UHYGroupManagerSubsystem* GroupManagerSubsystem = GetGameInstance()->GetSubsystem<UHYGroupManagerSubsystem>();
	if (GroupManagerSubsystem)
	{
		GroupManagerSubsystem->NotifyGroupAttacked(GroupId, DamageInstigator);
	}

	// Validation Check
	if (IsValid(HitActor))
	{
		USkeletalMeshComponent* Skeletal = HitActor->GetComponentByClass<USkeletalMeshComponent>();
		FVector BoneLocation;
		FName AttachPointName;
		AttachPointName = Skeletal->FindClosestBone(Hit.ImpactPoint, &BoneLocation);

		UGameplayStatics::SpawnDecalAttached(M_DarkEnergyDecal, BloodDecalSize, GetMesh(), AttachPointName,
			FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, 5.0f);

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, NS_DarkEnergy, BoneLocation,
			FRotator::ZeroRotator, HitParticleScale);
	}
	// Not implemented yet.
}

void AHYAggroNPCBase::ReturnAttackToken(int Amount)
{
	HPSystem->ReturnAttackToken(Amount);
}

bool AHYAggroNPCBase::ReserveAttackToken(int Amount)
{
	return HPSystem->ReserveAttackToken(Amount);
}

int32 AHYAggroNPCBase::SetCharacterState(ECharacterState InCharacterState, bool IsAddMode)
{
	// Is the state added?
	if (IsAddMode)
	{
		CharacterState = CharacterState | static_cast<uint8>(InCharacterState);
	}
	else
	{
		CharacterState = CharacterState & !static_cast<uint8>(InCharacterState);
	}

	return CharacterState;
}

void AHYAggroNPCBase::StoreAttackTokens(AActor* AttackTarget, int Amount)
{
	if (ReservedAttackTokens.Contains(AttackTarget))
	{
		Amount = Amount + ReservedAttackTokens[AttackTarget];
	}

	ReservedAttackTokens[AttackTarget] = Amount;
	return;
}

#pragma region Teammates Did(Already Implemented)
void AHYAggroNPCBase::AttackAnimStart()
{
	CombatSystem->bIsAttacking = true;
}

void AHYAggroNPCBase::AttackAnimEnd()
{
	CombatSystem->bIsAttacking = false;
}

void AHYAggroNPCBase::DeathDissolve()
{
	TArray<UMaterialInterface*> Materials = GetMesh()->GetMaterials();

	for (int i = 0; i < Materials.Num(); ++i)
	{
		DissolveMeshMaterials.Add(GetMesh()->CreateDynamicMaterialInstance(i, DeathMaterial));
	}

	DissolveTimeline.PlayFromStart();
}

void AHYAggroNPCBase::OnDissolvePlayed(float DeltaTime)
{
	for (auto DissolveMat : DissolveMeshMaterials)
	{
		DissolveMat->SetScalarParameterValue(TEXT("DissolveFactor"), FMath::Lerp(-1.0f, 1.0f, DeltaTime));
	}
}

void AHYAggroNPCBase::OnDissolveFinished()
{
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);

	auto PoolSystem = GetGameInstance()->GetSubsystem<UHYPoolSubSystem>();

	for (int i = AttachedActors.Num(); i >= 0; --i)
	{
		if (IsValid(AttachedActors[i]))
		{
			PoolSystem->ReturnToPool(AttachedActors[i]);
		}
	}

	AActor* SpawnedActor;
	PoolSystem->SpawnFromPool(PickupSoul, GetActorLocation(), GetActorRotation(), SpawnedActor);
	Destroy();
}

bool AHYAggroNPCBase::TakeDamageHelper(FDamageInfo DamageInfo, AActor* DamageInstigator, const FHitResult& Hit)
{
	StartHitStop(0.1f);
	bool bResult = false;

	if ((CharacterState & static_cast<uint8>(ECharacterState::Vulnerable)) != 0)
	{
		DamageInfo.DamageAmount *= 1.5f;
	}
	bResult = HPSystem->TakeDamage(DamageInfo, DamageInstigator, Hit);

	if (bResult)
	{
		// Hit VFX
		UNiagaraFunctionLibrary::SpawnSystemAttached(HitEffects[static_cast<int>(DamageInfo.DamageType)], 
			HitVFXPoint, TEXT("None"), FVector(0, 0, 0), FRotator(0, 0, 0), EAttachLocation::KeepRelativeOffset, true);

		UFMODBlueprintStatics::PlayEventAttached(HitEvents[static_cast<int>(DamageInfo.DamageType)], HitVFXPoint, TEXT(""),
			FVector(0, 0, 0), EAttachLocation::KeepRelativeOffset, false, true, true);

		UpdateHPBar();
		
		auto PoolSystem = GetGameInstance()->GetSubsystem<UHYPoolSubSystem>();

		AActor* SpawnedActor;
		PoolSystem->SpawnFromPool(DamageAmountClass, GetActorLocation(), GetActorRotation(), SpawnedActor);
		// Damage Amount inspector
		FActorSpawnParameters spawnParams;
		auto DamageAmount = Cast<AHYDamageAmount>(SpawnedActor);

		DamageAmount->Initialize(DamageInfo.DamageAmount, DamageInfo.DamageType);
	}

	return bResult;
}

void AHYAggroNPCBase::StartHitStop(float _Duration)
{
	CustomTimeDilation = 0.0f;

	GetWorld()->GetTimerManager().SetTimer(HitStopTimer, this, &AHYAggroNPCBase::EndHitStop, _Duration);
}

void AHYAggroNPCBase::EndHitStop()
{
	CustomTimeDilation = 1.0f;
}

void AHYAggroNPCBase::StopDOT()
{
	HPSystem->StopDOT();
	GetWorld()->GetTimerManager().ClearTimer(DOTTimer);
}

bool AHYAggroNPCBase::IsAttacking()
{
	return CombatSystem->GetIsAttacking();
}

float AHYAggroNPCBase::GetMaxHP()
{
	return HPSystem->GetMaxHP();
}

float AHYAggroNPCBase::GetCurrentHP()
{
	return HPSystem->GetCurrentHP();
}

bool AHYAggroNPCBase::IsDead()
{
	return HPSystem->GetIsDead();
}

float AHYAggroNPCBase::Heal(float AmountToHeal)
{
	return HPSystem->Heal(AmountToHeal);
}

bool AHYAggroNPCBase::TakeDamage(FDamageInfo DamageInfo, AActor* DamageInstigator, float DamageDuration, float DamageInterval, const FHitResult& Hit)
{
	switch (DamageInfo.DamageType)
	{
		// Non DOT Damage types
		case EDamageType::None:
		case EDamageType::CloseCombat:
		case EDamageType::NormalMeleeSlash:
		case EDamageType::SpiritMeleeSlash:
		case EDamageType::Projectile:
		case EDamageType::Parry:
		case EDamageType::Stun:
		case EDamageType::HealthDrain:
		case EDamageType::ElectricShock:
		case EDamageType::HaesolArrowSkill:
		case EDamageType::AOE:
		{
			return TakeDamageHelper(DamageInfo, DamageInstigator, Hit);
		}
			break;
		// DOT Damage types
		case EDamageType::Drown:
		{
			SetCharacterState(ECharacterState::Slowed, true);
			SetCharacterState(ECharacterState::Vulnerable, true);
			SetMovementSpeed(CurrentMovementType);
		}
		case EDamageType::Burn:
		case EDamageType::Poison:
		case EDamageType::Bleed:
		case EDamageType::Freeze:
		{
			GetWorld()->GetTimerManager().SetTimer(DOTTimer, this, &AHYAggroNPCBase::StopDOT, DamageDuration, false);
			HPSystem->StartDOT(DamageInfo, DamageInstigator, DamageInterval);
			return true;
		}
			break;
	}
	return false;
}

bool AHYAggroNPCBase::AttackStart(AActor* AttackTarget, int TokensNeeded)
{
	bool bResult = false;

	if (IDamagable* Damagable = Cast<IDamagable>(AttackTarget))
	{
		bResult = Damagable->ReserveAttackToken(TokensNeeded);
	}

	if (bResult)
	{
		StoreAttackTokens(AttackTarget, TokensNeeded);
		TokenUsedInCurrentAttack = TokensNeeded;
	}

	return bResult;
}

void AHYAggroNPCBase::AttackEnd(AActor* AttackTarget)
{
	if (IDamagable* Damagable = Cast<IDamagable>(AttackTarget))
	{
		Damagable->ReturnAttackToken(TokenUsedInCurrentAttack);
		StoreAttackTokens(AttackTarget, -TokenUsedInCurrentAttack);
		OnAttackEnd.Broadcast();
	}
}

float AHYAggroNPCBase::SetMovementSpeed(ENPCMovementSpeed MovementType)
{
	CurrentMovementType = MovementType;
	float Speed = 0.0f;

	switch (CurrentMovementType)
	{
	case ENPCMovementSpeed::Idle:
	{
		Speed = 0.0f;
	}
	break;
	case ENPCMovementSpeed::Walking:
	{
		Speed = 150.0f;
	}
	break;
	case ENPCMovementSpeed::Jogging:
	{
		Speed = 200.0f;
	}
	break;
	case ENPCMovementSpeed::Sprinting:
	{
		Speed = 300.0f;
	}
	break;
	}

	// Slow debuff
	if ((CharacterState & static_cast<uint8>(ECharacterState::Slowed)) != 0)
	{
		Speed = Speed * 0.4f;
	}

	GetCharacterMovement()->MaxWalkSpeed = Speed;

	return Speed;
}
#pragma endregion
