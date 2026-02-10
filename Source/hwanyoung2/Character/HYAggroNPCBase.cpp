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

void AHYAggroNPCBase::BeginPlay()
{
	Super::BeginPlay();

	GetLoreDataFromInstance();

	// Set up health bar widget.
	UUserWidget* NPCHealthBarHUD = CreateWidget<UUserWidget>(this, NPCHealthWidget);
	NPCHealthBarHUD->SetDamagableActor(this);
	Widget->SetWidget(NPCHealthBarHUD);
	NPCHealthBarHUD->SetPercentage();
	Widget->SetVisibility(false);

	// Set up controller instance and current state
	AICBase = Cast<AHYAggroNPCControllerBase>(UAIBlueprintHelperLibrary::GetAIController(this));
	SetCharacterState(ECharacterState::None, true);

	UHYGroupManagerSubsystem* GroupManagerSubsystem = GetGameInstance()->GetSubsystem<UHYGroupManagerSubsystem>();
	if (GroupManagerSubsystem)
	{
		GroupManagerSubsystem->RegisterGroup(this, GroupId);
	}

	// Set up binding to events
	HPSystem->OnDeath.AddDynamic(this, &AHYAggroNPCBase::Death_Implementation);
	HPSystem->OnDamageTaken.AddDynamic(this, &AHYAggroNPCBase::DamageTaken);

	//DialogueSystem->OnDialogueFinished.AddDynamic(this, &AHYAggroNPCBase::EndDialogue);
	CombatSystem->OnTraceDamage.AddDynamic(this, &AHYAggroNPCBase::DealDamage);
}

void AHYAggroNPCBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHYAggroNPCBase::Death_Implementation()
{
	// Do when npc is dead
	if (IsDead())
	{
		// Stop behavior
		AICBase->GetBrainComponent()->StopLogic(TEXT(""));

		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		TArray<AActor*> ActorsToReturnTokens;
		ReservedAttackTokens.GetKeys(ActorsToReturnTokens);

		// Return current taken tokens
		for (auto ActorAndToken : ReservedAttackTokens)
		{
			Cast<AHYPlayerCharacterBase>(ActorAndToken.Key)->ReturnAttackToken(ActorAndToken.Value);
		}
	}
}

void AHYAggroNPCBase::Parried_Implementation()
{
	//AICBase->SetStateAsFrozen();
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	// Remove current attack target
	AICBase->SetStateAsFrozen();


	if (AnimInstance && StaggerMontage)
	{
		FOnMontageEnded EndDelegate;
		EndDelegate.BindLambda([&](UAnimMontage* _AnimMontage, bool _IsEnded)
		{
			AICBase->SetStateAsAttacking(AICBase->GetAttackTarget(), false);
		});
		AnimInstance->Montage_SetEndDelegate(EndDelegate);
		

		AnimInstance->Montage_Play(StaggerMontage);
		//PlayAnimMontage(StaggerMontage);
	}
}

void AHYAggroNPCBase::GetLoreDataFromInstance_Implementation()
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
		
		// Damage Amount inspector
		FActorSpawnParameters spawnParams;
		ADamageAmount* DamageAmount = GetWorld()->SpawnActor<ADamageAmount>(DamageAmountClass, 
			GetActorTransform(), spawnParams);

		DamageAmount->SetDamageAmount(DamageInfo.DamageAmount);
		DamageAmount->SetDamageType(DamageInfo.DamageType);
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

void AHYAggroNPCBase::NotifyGroupAttacked(AActor* AttackTarget)
{
	AICBase->NoticeEnemy(AttackTarget);
}

void AHYAggroNPCBase::ToggleParryTiming(bool IsParryable)
{
	bCanBeParried = IsParryable;
}

void AHYAggroNPCBase::StopDOT()
{
	HPSystem->StopDOT_Implementation();
	GetWorld()->GetTimerManager().ClearTimer(DOTTimer);
}

void AHYAggroNPCBase::ReturnAttackToken(int _Amount)
{
	HPSystem->ReturnAttackToken(_Amount);
}

bool AHYAggroNPCBase::ReserveAttackToken(int _Amount)
{
	return HPSystem->ReserveAttackToken(_Amount);
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

float AHYAggroNPCBase::Heal(float _AmountToHeal)
{
	return HPSystem->Heal(_AmountToHeal);
}

bool AHYAggroNPCBase::TakeDamage(FDamageInfo _DamageInfo, AActor* _DamageInstigator, float _DamageDuration, float _DamageInterval, const FHitResult& _Hit)
{
	switch (_DamageInfo.DamageType)
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
			return TakeDamageHelper(_DamageInfo, _DamageInstigator, _Hit);
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
			GetWorld()->GetTimerManager().SetTimer(DOTTimer, this, &AHYAggroNPCBase::StopDOT, _DamageDuration, false);
			HPSystem->StartDOT_Implementation(_DamageInfo, _DamageInstigator, _DamageInterval);
			return true;
		}
			break;
	}
	return false;
}

int32 AHYAggroNPCBase::SetCharacterState(ECharacterState _InCharacterState, bool _IsAddMode)
{
	// Is the state added?
	if (_IsAddMode)
	{
		CharacterState = CharacterState | static_cast<uint8>(_InCharacterState);
	}
	else
	{
		CharacterState = CharacterState & !static_cast<uint8>(_InCharacterState);
	}

	return CharacterState;
}

bool AHYAggroNPCBase::AttackStart(AActor* _AttackTarget, int _TokensNeeded)
{
	bool bResult = Cast<AHYPlayerCharacterBase>(_AttackTarget)->ReserveAttackToken(_TokensNeeded);

	if (bResult)
	{
		StoreAttackTokens(_AttackTarget, _TokensNeeded);
		TokenUsedInCurrentAttack = _TokensNeeded;
	}

	return bResult;
}

void AHYAggroNPCBase::AttackEnd(AActor* _AttackTarget)
{
	Cast<AHYPlayerCharacterBase>(_AttackTarget)->ReturnAttackToken(TokenUsedInCurrentAttack);
	StoreAttackTokens(_AttackTarget, TokenUsedInCurrentAttack - 1);
	OnAttackEnd.Broadcast();
}

void AHYAggroNPCBase::StoreAttackTokens(AActor* _AttackTarget, int _Amount)
{
	if (ReservedAttackTokens.Contains(_AttackTarget))
	{
		_Amount = _Amount + ReservedAttackTokens[_AttackTarget];
	}
	
	ReservedAttackTokens[_AttackTarget] = _Amount;
	return;
}

float AHYAggroNPCBase::SetMovementSpeed(ENPCMovementSpeed _MovementType)
{
	CurrentMovementType = _MovementType;
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

void AHYAggroNPCBase::EndDialogue(AHYPlayerCharacterBase* _PlayerRef)
{
	OnDialogueEnd.Broadcast();
}

void AHYAggroNPCBase::StartDialogue(AHYPlayerCharacterBase* _PlayerRef)
{
	DialogueSystem->StartDialogue(_PlayerRef);
}

void AHYAggroNPCBase::UpdateHPBar()
{
	UUserWidget* NPCHealthBarWidget = Cast<UUserWidget>(Widget->GetUserWidgetObject());

	NPCHealthBarWidget->SetPercentage();
}