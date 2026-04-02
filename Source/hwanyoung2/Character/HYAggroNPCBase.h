// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HYEnemyBase.h"
#include "Interfaces/CharacterState.h"
#include "Interfaces/NPCBehavior.h"
#include "Interfaces/CombatSystem.h"
#include "Interfaces/Dialogueable.h"
#include "Interfaces/Damagable.h"
#include "Interfaces/Buffable.h"
#include "Components/TimelineComponent.h"
#include "HYAggroNPCBase.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponEquipped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponUnequipped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHPChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueEnd);
/**
 * 
 */
UCLASS()
class HWANYOUNG2_API AHYAggroNPCBase : public AHYEnemyBase, public ICharacterState, public INPCBehavior, public ICombatSystem, public IDialogueable, public IDamagable, public IBuffable
{
	GENERATED_BODY()

public:
	AHYAggroNPCBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

#pragma region Events in EventGraph
	UFUNCTION(BlueprintCallable)
	virtual void Death(AActor* DamageInstigator);

	// Called when player succeed to parry
	UFUNCTION(BlueprintCallable)
	virtual void Parried();

	UFUNCTION(BlueprintCallable)
	virtual void GetLoreDataFromInstance();

	UFUNCTION(BlueprintCallable)
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintCallable)
	virtual void InitAggroNPCState();

	UFUNCTION(BlueprintCallable)
	virtual void DamageTaken(EDamageReactionType DamageResponse, FCrowdControlInfo CrowdControlInfo, AActor* DamageInstigator, const FHitResult& Hit);

#pragma region Teammates Did(Already Implemented)
	UFUNCTION(BlueprintCallable)
	virtual void DealDamage(AActor* HitActor, const FHitResult& HitResult) {}
	UFUNCTION(BlueprintCallable)
	virtual void AttackAnimStart();

	UFUNCTION(BlueprintCallable)
	virtual void AttackAnimEnd();

	UFUNCTION(BlueprintCallable)
	virtual void DeathDissolve();

	UFUNCTION(BlueprintCallable)
	virtual void OnDissolvePlayed(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	virtual void OnDissolveFinished();

#pragma endregion

#pragma endregion

#pragma region Functions

#pragma region Teammates Did(Already Implemented)
	// Only works for Non DOT Damage
	UFUNCTION(BlueprintCallable)
	virtual bool TakeDamageHelper(FDamageInfo DamageInfo, AActor* DamageInstigator, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category = "Hit Event")
	virtual void StartHitStop(float Duration);

	UFUNCTION(BlueprintCallable, Category = "Hit Event")
	virtual void EndHitStop();
#pragma endregion

	UFUNCTION(BlueprintCallable, Category = "AI|Group Battle")
	virtual void NotifyGroupAttacked(AActor* AttackTarget);

	UFUNCTION(BlueprintCallable, Category = "Parry")
	virtual void ToggleParryTiming(bool IsParryable);
#pragma endregion

#pragma region Interfaces
	/// <summary>
	/// Apply Buff to NPC.
	/// </summary>
	/// <param name="BuffInfo">BuffInfo has all the information about the buff, such as buff type, duration, magnitude, etc.</param>
	/// <returns>Is buff applied</returns>
	UFUNCTION(BlueprintCallable)
	virtual bool Buff(FBuffableInfo BuffInfo) override;

	/// <summary>
	/// Remove buff from NPC.
	/// </summary>
	/// <param name="BuffInfo">BuffInfo has all the information about the buff, such as buff type, duration, magnitude, etc.</param>
	/// <returns>Is buff removed</returns>
	UFUNCTION(BlueprintCallable)
	virtual bool RemoveBuff(FBuffableInfo BuffInfo) override;

	/// <summary>
	/// Apply debuff to NPC.
	/// </summary>
	/// <param name="BuffInfo">BuffInfo has all the information about the buff, such as buff type, duration, magnitude, etc.</param>
	/// <returns>Is debuff applied</returns>
	UFUNCTION(BlueprintCallable)
	virtual bool Debuff(FBuffableInfo BuffInfo) override;

	/// <summary>
	/// Remove debuff from NPC.
	/// </summary>
	/// <param name="BuffInfo">BuffInfo has all the information about the buff, such as buff type, duration, magnitude, etc.</param>
	/// <returns>Is debuff removed</returns>
	UFUNCTION(BlueprintCallable)
	virtual bool RemoveDebuff(FBuffableInfo BuffInfo) override;

#pragma region Teammates Did(Already Implemented)
	UFUNCTION(BlueprintCallable)
	virtual void StopDOT() override;

	UFUNCTION(BlueprintCallable)
	virtual bool IsAttacking() override;

	UFUNCTION(BlueprintCallable)
	virtual float GetMaxHP() override;

	UFUNCTION(BlueprintCallable)
	virtual float GetCurrentHP() override;

	UFUNCTION(BlueprintCallable)
	virtual bool IsDead() override;

	UFUNCTION(BlueprintCallable)
	virtual AActor* GetPatrolRoute() override { return nullptr; }

	UFUNCTION(BlueprintCallable)
	virtual void GetIdealRange(float& AttackRadius, float& DefenseRadius) override {}

	UFUNCTION(BlueprintCallable)
	virtual void Attack(AActor* AttackTarget) override {}

	virtual void EquipWeapon() override {}

	virtual void UnequipWeapon() override {}

	virtual bool ChangeDefaultAttackMode(EDefaultAttackMode IncommingMode) override { return true; }

	virtual void ToggleWeaponTrace(bool WeaponTraceOn) override {}

	// Heal Current HP
	UFUNCTION(BlueprintCallable)
	float Heal(float AmountToHeal) override;

	// Take Damage to HP system
	UFUNCTION(BlueprintCallable)
	virtual bool TakeDamage(FDamageInfo DamageInfo, AActor* DamageInstigator, float DamageDuration, float DamageInterval, const FHitResult& Hit) override;

	UFUNCTION(BlueprintCallable)
	virtual bool AttackStart(AActor* AttackTarget, int TokensNeeded) override;

	// Called when finishing attack
	UFUNCTION(BlueprintCallable)
	void AttackEnd(AActor* AttackTarget);

	UFUNCTION(BlueprintCallable)
	virtual USkeletalMeshComponent* GetCharacterMesh() override { return GetMesh(); }

#pragma endregion

	UFUNCTION(BlueprintCallable)
	virtual void ReturnAttackToken(int Amount) override;

	UFUNCTION(BlueprintCallable)
	virtual bool ReserveAttackToken(int Amount) override;
	// HP Recharge Passive

	UFUNCTION(BlueprintCallable)
	virtual bool IsActivatedHPRecharge() override { return false; }

	// Character State Getter & Setter
	UFUNCTION(BlueprintCallable)
	virtual int32 SetCharacterState(ECharacterState InCharacterState, bool IsAddMode) override;

	UFUNCTION(BlueprintCallable)
	virtual int32 GetCharacterState() override { return static_cast<int32>(CharacterState); }

	UFUNCTION(BlueprintCallable)
	virtual void StoreAttackTokens(AActor* AttackTarget, int Amount) override;

	UFUNCTION(BlueprintCallable)
	virtual float SetMovementSpeed(ENPCMovementSpeed MovementType) override;

	UFUNCTION(BlueprintCallable)
	virtual void EndDialogue(AHYPlayerCharacterBase* PlayerRef) override;

	UFUNCTION(BlueprintCallable)
	virtual void StartDialogue(AHYPlayerCharacterBase* PlayerRef) override;

	UFUNCTION(BlueprintCallable)
	virtual void UpdateHPBar();
#pragma endregion


#pragma region Components
	// Actor component for managing dialogue
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="Components")
	TObjectPtr<class UHYDialogueComponent> DialogueSystem;

	// Manage location to show hit VFX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USceneComponent> HitVFXPoint;

	// Widget component for HP Bar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UWidgetComponent> Widget;

	// Actor component of combat system for managing NPC's weapon(it is WeaponMechanism of AggorNPC)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UHYCombatSystem> CombatSystem;

	// HP System
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UHYHPSystem> HPSystem;

	// Attribute System
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UHYAttributeSystem> AttributeSet;

#pragma endregion

#pragma region Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<AActor> PatrolRoute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<class UBehaviorTree> BehaviorTree;

	// AI Controller
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Group Battle")
	TObjectPtr<class AHYAIController> AICBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Group Battle")
	TMap<AActor*, int> ReservedAttackTokens;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Group Battle")
	int TokenUsedInCurrentAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Group Battle")
	FName GroupId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State", meta = (Bitmask, BitmaskEnum="ECharacterState"))
	uint8 CharacterState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bIsWeldingWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<class AHYWeaponBase> WeaponActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float DamageAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Name")
	FName BossNPCRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	ENPCMovementSpeed CurrentMovementType;

	// Animation for stagger
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimMontage> StaggerMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation|Death")
	TArray<UMaterialInstanceDynamic*> DissolveMeshMaterials;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bCanBeParried;

	// Blood Hit VFX(Decal)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood")
	TObjectPtr<UMaterialInterface> M_DarkEnergyDecal;

	// Blood Hit VFX(Particles)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood")
	class UNiagaraSystem* NS_DarkEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood")
	FVector BloodDecalSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood")
	FVector HitParticleScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit_VFX")
	TArray<UNiagaraSystem*> NS_Buffables;

	// This is for character state. It has int value for each state, and it can be used for various purposes such as calculating damage, changing behavior, etc.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat")
	TMap<ECharacterState, int> StatStance;

	// The timer handle for each character state. It can be used for various purposes such as calculating damage over time, changing behavior after certain time, etc.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	TMap<ECharacterState, FTimerHandle> StanceTimer;

	// The particles for each character state.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	TMap<ECharacterState, class UNiagaraComponent*> StanceParticles;

	// For managing all debuffs(to remove them using cleanse)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Buffable")
	TArray<FAppliedDebuffInfo> AppliedStatDebuffs;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	FTimerHandle DOTTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	FTimerHandle EndTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	FTimerHandle HitStopTimer;
#pragma endregion

#pragma region resource
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	TSubclassOf<class UUserWidget> NPCHealthWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	TArray<TObjectPtr<class UNiagaraSystem>> HitEffects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	TArray<TObjectPtr<class UFMODEvent>> HitEvents;

	// Damage Amount class for spawning.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	TSubclassOf<class AActor> DamageAmountClass;
	//TSubclassOf<class ADamageAmount> DamageAmountClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	TObjectPtr<class UAnimMontage> PlayerStaggerMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	TObjectPtr<UMaterialInterface> DeathMaterial;

	FTimeline DissolveTimeline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Resource")
	UCurveFloat* DissolveCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Resource")
	TSubclassOf<AActor> PickupSoul;

#pragma endregion
	
public:
#pragma region Event Dispatcher
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnWeaponEquipped OnWeaponEquipped;

	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnWeaponUnequipped OnWeaponUnequipped;

	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnAttackEnd OnAttackEnd;

	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnHPChanged OnHPChanged;

	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnDeath OnDeath;

	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnDialogueEnd OnDialogueEnd;
#pragma endregion

};
