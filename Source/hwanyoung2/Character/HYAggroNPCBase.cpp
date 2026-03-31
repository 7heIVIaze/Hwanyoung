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
	bool bResult = Cast<AHYPlayerCharacterBase>(AttackTarget)->ReserveAttackToken(TokensNeeded);

	if (bResult)
	{
		StoreAttackTokens(AttackTarget, TokensNeeded);
		TokenUsedInCurrentAttack = TokensNeeded;
	}

	return bResult;
}

void AHYAggroNPCBase::AttackEnd(AActor* AttackTarget)
{
	Cast<AHYPlayerCharacterBase>(AttackTarget)->ReturnAttackToken(TokenUsedInCurrentAttack);
	StoreAttackTokens(AttackTarget, TokenUsedInCurrentAttack - 1);
	OnAttackEnd.Broadcast();
}

float AHYAggroNPCBase::SetMovementSpeed(ENPCMovementSpeed MovementType)
{
	CurrentMovementType = MovementType;
	float fSpeed = 0.0f;

	switch (CurrentMovementType)
	{
	case ENPCMovementSpeed::Idle:
	{
		fSpeed = 0.0f;
	}
	break;
	case ENPCMovementSpeed::Walking:
	{
		fSpeed = 150.0f;
	}
	break;
	case ENPCMovementSpeed::Jogging:
	{
		fSpeed = 200.0f;
	}
	break;
	case ENPCMovementSpeed::Sprinting:
	{
		fSpeed = 300.0f;
	}
	break;
	}

	// Slow debuff
	if ((CharacterState & static_cast<uint8>(ECharacterState::Slowed)) != 0)
	{
		fSpeed = fSpeed * 0.4f;
	}

	GetCharacterMovement()->MaxWalkSpeed = fSpeed;

	return fSpeed;
}
#pragma endregion
