// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "hwanyoung2Character.h"
#include "Interfaces/CharacterState.h"
#include "Interfaces/Buffable.h"
#include "Interfaces/Damagable.h"
#include "Interfaces/ManaUser.h"
#include "Components/TimelineComponent.h"
#include "Inventory/InventoryItem.h"
#include "HYPlayerCharacterBase.generated.h"

/**
 *
 */
enum EWeaponIndex
{
	None, // Dummy Index
	Melee, // Melee weapon(Sword)
	Projectile, // Projectile weapon(Bow&Arrow, ...)
	MartialArts,

};

enum ESkillIndex
{
	E, // E Skill Index
	Q, // Q Skill Index
	C, // C Skill Index
	Z, // Z Skill Index
	ALL, // To prevent skill casting
};

UCLASS()
class HWANYOUNG2_API AHYPlayerCharacterBase : public Ahwanyoung2Character, public ICharacterState, public IBuffable, public IDamagable, public IManaUser
{
	GENERATED_BODY()

public:
	AHYPlayerCharacterBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = System, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UHYMPSystem> MPSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = System, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UHYAttributeSystem> AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = System, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UHYWeaponMechanism> WeaponMechanism;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = System, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UHYHPSystem> HPSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileTrace, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> Sphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileTrace, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UDecalComponent> Decal;


protected:
	// An event that occurs when character is in the air and then fall to the ground (landing, etc.)

	virtual void BeginPlay();

	virtual void Tick(float DeltaTime);

	virtual void OnLanded(const FHitResult& Hit);

public:

#pragma region Getter
	// Get current weapon index
	UFUNCTION(BlueprintCallable)
	int32 GetWeaponIndex() { return WeaponIndex; }

	UFUNCTION(BlueprintCallable)
	virtual float GetMaxHP() override;

	UFUNCTION(BlueprintCallable)
	virtual float GetCurrentHP() override;

	UFUNCTION(BlueprintCallable)
	virtual float GetMaxMana() override;

	UFUNCTION(BlueprintCallable)
	virtual float GetCurrentMana() override;

	UFUNCTION(BlueprintCallable)
	virtual float GetCurrentUltPoints() override;

	UFUNCTION(BlueprintCallable)
	virtual float GetMaxUltPoints() override;

	UFUNCTION(BlueprintCallable)
	virtual FText GetCharacterName() { return CharacterName; }
#pragma endregion

#pragma region EventGraphs
	// Handling death of the character (physics)
	UFUNCTION(BlueprintNativeEvent)
	void Death();
	
	// Handling death of the character(physics)
	virtual void Death_Implementation();
	// Toggle Inventory Widget
	UFUNCTION(BlueprintNativeEvent)	// Declare as NativeEvent so that it can be changed in blueprint.
	void ToggleInventory();

	virtual void ToggleInventory_Implementation();

	// Dash Skill
	UFUNCTION(BlueprintNativeEvent)	// Declare as NativeEvent so that it can be changed in blueprint.
	void Dash(float DeltaTime);

	virtual void Dash_Implementation(float DeltaTime);

	// Prevent skill use for preventing skill casted.
	UFUNCTION(BlueprintNativeEvent)	// Declare as NativeEvent so that it can be changed in blueprint.
		void PreventSkillCasting(int CastedSkill);

	virtual void PreventSkillCasting_Implementation(int CastedSkill);

	// Teleport to Base camp; This function will be called by HaeTae Actor(Save Actor)
	UFUNCTION(BlueprintNativeEvent)	// Declare as NativeEvent so that it can be changed in blueprint.
		void Teleport(FVector TeleportLocation);

	virtual void Teleport_Implementation(FVector TeleportLocation);

	// A function that implements fall damage. The actual implementation will be done by Implementation.
	UFUNCTION(BlueprintNativeEvent)	// Declare as NativeEvent so that it can be changed in blueprint.
		void FallDamageEvent();

	// A function that implements the part that actually deals fall damage.
	virtual void FallDamageEvent_Implementation();

	UFUNCTION(BlueprintCallable)
	virtual void OnDash(float Time);

	UFUNCTION(BlueprintCallable)
	virtual void DashFinished();

	// Called when timeline play.
	// @param : _Value is curve vector. _Value.X is for Dissolve, _Value.Y is for Strength_VN
	UFUNCTION(BlueprintCallable)
	virtual void OnTeleport(FVector Value);

	// Respond damage reaction such as knocked back, airborne, stunned, stagger, what else.
	UFUNCTION(BlueprintNativeEvent)
	void DamageResponse(EDamageReactionType DamageResponse, FCrowdControlInfo CrowdControl,
					AActor* DamageInstigator, const FHitResult& Hit);

	virtual void DamageResponse_Implementation(EDamageReactionType DamageResponse, 
					FCrowdControlInfo CrowdControl, AActor* DamageInstigator, const FHitResult& Hit);
	// Add Main HUD to viewport
	
	UFUNCTION(BlueprintNativeEvent)	// Declare as NativeEvent so that it can be changed in blueprint.
	void AddPlayerHUDWidgetToViewport();

	virtual void AddPlayerHUDWidgetToViewport_Implementation();

	// Remove Main HUD from viewport
	UFUNCTION(BlueprintNativeEvent)	// Declare as NativeEvent so that it can be changed in blueprint.
	void RemovePlayerHUDWidget();

	virtual void RemovePlayerHUDWidget_Implementation();

	// Interaction Function
	UFUNCTION(BlueprintNativeEvent)	// Declare as NativeEvent so that it can be changed in blueprint.
	void Interact();

	virtual void Interact_Implementation();

	// Parry Success Event
	UFUNCTION(BlueprintNativeEvent)
	void ParrySuccess(AActor* DamageInstigator);

	virtual void ParrySuccess_Implementation(AActor* DamageInstigator);

	// Parry Failed Event
	UFUNCTION(BlueprintNativeEvent)
	void ParryFailed(AActor* DamageInstigator);

	virtual void ParryFailed_Implementation(AActor* DamageInstigator);

	// Q Skill Cooldown
	UFUNCTION(BlueprintNativeEvent)
	void QSkillCooldown();

	virtual void QSkillCooldown_Implementation();

	UFUNCTION(BlueprintCallable)
	virtual void QSkillCooldownUpdated(float LeftCooldown);

	UFUNCTION(BlueprintCallable)
	virtual void QSkillCooldownFinished();

	UFUNCTION(BlueprintCallable)
	virtual void ResetQSkill();

	// E Skill Cooldown
	UFUNCTION(BlueprintNativeEvent)
	void ESkillCooldown();

	virtual void ESkillCooldown_Implementation();

	UFUNCTION(BlueprintCallable)
	virtual void ESkillCooldownUpdated(float LeftCooldown);

	UFUNCTION(BlueprintCallable)
	virtual void ESkillCooldownFinished();

	UFUNCTION(BlueprintCallable)
	virtual void ResetESkill();

	// C Skill Ability
	UFUNCTION(BlueprintNativeEvent)
	void UseThirdAbility();

	virtual void UseThirdAbility_Implementation();

	// C Skill Cooldown
	UFUNCTION(BlueprintNativeEvent)
	void CSkillCooldown();

	virtual void CSkillCooldown_Implementation();

	UFUNCTION(BlueprintCallable)
	virtual void CSkillCooldownUpdated(float LeftCooldown);

	UFUNCTION(BlueprintCallable)
	virtual void CSkillCooldownFinished();

	UFUNCTION(BlueprintCallable)
	virtual void ResetCSkill();

	// Z Skill Ability
	UFUNCTION(BlueprintNativeEvent)
	void UseUltimateAbility();

	virtual void UseUltimateAbility_Implementation();

	// Z Skill Cooldown
	UFUNCTION(BlueprintNativeEvent)
	void UltSkillCooldown();

	virtual void UltSkillCooldown_Implementation();

	UFUNCTION(BlueprintCallable)
	virtual void UltSkillCooldownUpdated(float LeftCooldown);

	UFUNCTION(BlueprintCallable)
	virtual void UltSkillCooldownFinished();

	UFUNCTION(BlueprintCallable)
	virtual void ResetUltSkill();

	// Parry/Blocked Event logic(this function will be bound with OnDamageBlocked in HP System)
	UFUNCTION(BlueprintNativeEvent)
	void Blocked(bool CanBeParried, AActor* DamageInstigator);

	virtual void Blocked_Implementation(bool CanBeParried, AActor* DamageInstigator);

#pragma region For Player HUD Update
	// For update hp bar in player hud
	UFUNCTION(BlueprintCallable)
	virtual void UpdateHPBar();

	// For update mp bar in player hud
	UFUNCTION(BlueprintCallable)
	virtual void UpdateManaBar();

	// For update ultimate points bar in player hud
	UFUNCTION(BlueprintCallable)
	virtual void UpdateUltimatePointsBar();

	// For update honbaek counts in player hud
	UFUNCTION(BlueprintCallable)
	virtual void UpdateHonbaek(int Amount);
#pragma endregion
#pragma endregion

#pragma region Functions
	UFUNCTION(BlueprintCallable)
	virtual void OnMontageEnded(UAnimMontage* Montage, bool Interrupted);

	UFUNCTION(BlueprintCallable)
	virtual void StartHitStop(float Duration);

	UFUNCTION(BlueprintCallable)
	virtual void EndHitStop();

	virtual void UseStamina() override;

	virtual void ReplenishStamina() override;

	UFUNCTION(BlueprintCallable, Category = "Death")
	virtual void DeathNofity();

	UFUNCTION(BlueprintCallable, Category = "Death")
	virtual void OnDeathTimelinePlayed(float DeltaTime);

#pragma endregion

#pragma region Interfaces
	UFUNCTION(BlueprintCallable)
	virtual bool IsDead() override;

	// Heal Current HP
	UFUNCTION(BlueprintCallable)
	float Heal(float AmountToHeal) override;

	// Use Ultimate Points
	UFUNCTION(BlueprintCallable)
	virtual bool UseUltimatePoints(float ConsumedUltPoints) override;

	// Use Ultimate Points
	UFUNCTION(BlueprintCallable)
	virtual float AddUltimatePoints(float AddedUltPoints) override;

	// MP Recovery function.
	UFUNCTION(BlueprintCallable)
	float ReplenishMana(float AmountOfMana) override;

	// Use Mana for casting skill
	UFUNCTION(BlueprintCallable)
	bool UseMana(float ToBeConsumedMana) override;

	// MP Recharge Passive
	UFUNCTION(BlueprintCallable)
	void PassiveMPRecharge(bool IsCriticalDamage);

	// Character State Getter & Setter
	UFUNCTION(BlueprintCallable)
	virtual int32 SetCharacterState(ECharacterState InCharacterState, bool IsAddMode) override { 
		// Add buff or debuff state
		if (IsAddMode)
		{
			CharacterStance = CharacterStance | static_cast<int32>(InCharacterState);
			return CharacterStance;
		}
		// Remove buff or debuff state
		else
		{
			CharacterStance = CharacterStance & ~(static_cast<int32>(InCharacterState));
			return CharacterStance;
		}
	}

	UFUNCTION(BlueprintCallable)
	virtual int32 GetCharacterState() override { return CharacterStance; }

	// Return attack token for group battle
	UFUNCTION(BlueprintCallable)
	virtual void ReturnAttackToken(int Amount) override;

	// Reserve attack token for group battle
	UFUNCTION(BlueprintCallable)
	virtual bool ReserveAttackToken(int Amount) override;

	// Skill info Setter
	UFUNCTION(BlueprintCallable)
	virtual void SetSkillInfo(bool CanBeCast, int SkillIndex, int UsedSkillIndex);

	// Take Damage to HP system
	UFUNCTION(BlueprintCallable)
	virtual bool TakeDamage(FDamageInfo DamageInfo, AActor* DamageInstigator,
				float DamageDuration, float DamageInterval, const FHitResult& _Hit) override;

	// Stop Taking DOT Damage
	UFUNCTION(BlueprintCallable)
	virtual void StopDOT() override;


	UFUNCTION(BlueprintCallable)
	virtual bool ResetAttributes() override;

	// Buff character
	UFUNCTION(BlueprintCallable)
	virtual bool Buff(FBuffableInfo BuffableInfo) override;

	// Debuff character
	UFUNCTION(BlueprintCallable)
	virtual bool Debuff(FBuffableInfo BuffableInfo) override;

	UFUNCTION(BlueprintCallable)
	virtual bool RemoveBuff(FBuffableInfo BuffableInfo) override;

	// Debuff Expired character
	UFUNCTION(BlueprintCallable)
	virtual bool RemoveDebuff(FBuffableInfo BuffableInfo) override;

	// Remove all applied debuffs
	UFUNCTION(BlueprintCallable)
	virtual bool Cleanse() override;

	UFUNCTION(BlueprintCallable)
	virtual bool IsAttacking() override { return false; }

	// HP Recharge Passive
	UFUNCTION(BlueprintCallable)
	virtual bool IsActivatedHPRecharge() override { return false; }

	// MP Recharge Passive
	UFUNCTION(BlueprintCallable)
	virtual bool IsActivatedMPRecharge() override { return false; }

#pragma endregion
	//
	//// This Function will be called when player buy the item.
	//// @param Controller: Player Controller, ItemToBuy: the item structure(Data) to buy, Amount: the number of the item.
	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	//void BuyTradableItem(APlayerController* PlayerController, const FInventoryItem ItemToBuy, const int Amount);

	//// This Function will be called when player buy the item.
	//// @param Controller: Player Controller, ItemToBuy: the item structure(Data) to buy, Amount: the number of the item.
	//virtual void BuyTradableItem_Implementation(APlayerController* PlayerController, const FInventoryItem ItemToBuy, const int Amount);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USplineComponent> SplinePath;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<FInventoryItem, int> TradableItems;

	UPROPERTY(VisibleAnywhere, Category = "Character Ability")
	FTimeline DashTimeline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Ability")
	TObjectPtr<UCurveFloat> DashTimelineCurveFloat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Melee Combo")
	bool bIsWithinResumeComboWindow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Melee Combo")
	bool bAttacking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Melee Combo")
	bool bCanResumeCombo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parry")
	bool bIsParrying;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Melee Combo")
	float ComboFactor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Attack")
	float AimFOV;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Attack")
	float OriginalFOV;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Attack")
	FVector AimBoomOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Attack")
	FVector OriginalBoomOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Attack")
	FVector SkillAimBoomOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Ability")
	uint8 BlockingState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Ability")
	float ProjAOEDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Ability")
	TEnumAsByte<ECollisionChannel> ProjectilePathTraceChannel;

	// Skill cooldown timer.(Q)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Attack")
	FTimerHandle QSkillCooldownTimer;

	// Skill cooldown timer.(E)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Ability")
	FTimerHandle ESkillCooldownTimer;

	// Skill cooldown timer.(C)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Ability")
	FTimerHandle CSkillCooldownTimer;

	// Skill cooldown timer.(Z)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Ability")
	FTimerHandle UltSkillCooldownTimer;

	// Skill casting timer.(Q)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Attack")
	FTimerHandle QSkillCastingTimer;

	// Skill casting timer.(E)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Ability")
	FTimerHandle ESkillCastingTimer;

	// Skill casting timer.(C)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Ability")
	FTimerHandle CSkillCastingTimer;

	// Skill casting timer.(Z)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Ability")
	FTimerHandle UltSkillCastingTimer;


	// Player controller reference(HYPlayerCharacController)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI/UX")
	TObjectPtr<class AHYPlayerCharacController> ControllerRef;

	// Menu Open SFX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<class UFMODEvent> MenuOpenSound;

	// Dash SFX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<class UFMODEvent> DashSound;

	// Death animation montage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UAnimMontage> DeathMontage;

	// Player Main HUD
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<class UUserWidget> PlayerHUD;
	//TObjectPtr<class UPlayerHUDWidget> PlayerHUD;

	// Aim projectile HUD
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<class UUserWidget> PlayerCrosshair;
	//TObjectPtr<class UPlayerCrossHair> PlayerCrosshair;

	// Widget class of main HUD
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> PlayerHUDWidget;

	// Widget class of aim projectile HUD
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> PlayerCrosshairWidget;

	// Widget class of death widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> DeathWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<class UUserWidget> DeathWidget;


	// Blood gash.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit_VFX")
	UMaterialInterface* M_BloodGash;

	// the decal(Blood Gash) size
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit_VFX")
	FVector DecalSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit_VFX")
	class UNiagaraSystem* NS_Blood;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit_VFX")
	TArray<UNiagaraSystem*> NS_Buffables;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> ShopWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widget")
	TObjectPtr<class UUserWidget> ShopWidget;*/

protected:
	FTimeline TeleportTimeline;

	FTimeline DeathTimeline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Teleport", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveVector> TeleportTimelineCurveVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Teleport", meta = (AllowPrivateAccess = "true"))
	TArray<UMaterialInterface*> OriginalMeshMaterial;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Teleport", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* TeleportDissolve;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Teleport", meta = (AllowPrivateAccess = "true"))
	TArray<UMaterialInstanceDynamic*> DissolveMeshMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Dash", meta = (AllowPrivateAccess = "true"))
	bool bCanUseDash = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material", meta = (AllowPrivateAccess = "true"))
	class UMaterialParameterCollection* MPC_PlayerLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material", meta = (AllowPrivateAccess = "true"))
	class UMaterialParameterCollection* MPC_CircularProgressBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX", meta = (AllowPrivateAccess = "true"))
	class UFMODEvent* PlayerHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	FTimerHandle PlayerDOTDamageTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	FTimerHandle HitStopTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive", meta = (AllowPrivateAccess = "true"))
	bool bIsActivatedMPRecharge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State", meta = (AllowPrivateAccess = "true"))
	int32 CharacterStance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Ability", meta = (AllowPrivateAccess = "true"))
	TArray<FSkillInfo> SkillInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Ability", meta = (AllowPrivateAccess = "true"))
	float PreventCastingSkillTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Ability", meta = (AllowPrivateAccess = "true"))
	bool bIsActivatedGrit;

	// Current weapon index
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Ability", meta = (AllowPrivateAccess = "true"))
	int32 WeaponIndex;

	// Skill UI timeline
	FTimeline LockTimeline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Ability", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> LockTimelineCurveFloat;

	// Q Skill UI timeline
	FTimeline QSkillCooldownTimeline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Ability", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> QSkillTimelineCurveFloat;

	// E Skill UI timeline
	FTimeline ESkillCooldownTimeline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Ability", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> ESkillTimelineCurveFloat;

	// C Skill UI timeline
	FTimeline CSkillCooldownTimeline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Ability", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> CSkillTimelineCurveFloat;

	// Z Skill UI timeline
	FTimeline ZSkillCooldownTimeline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Ability", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> ZSkillTimelineCurveFloat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Ability", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> DeathTimelineCurveFloat;

	// For managing status buffs/debuffs.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Buffable", meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterState, int> StatStances;

	// For managing crowd control debuffs.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Buffable", meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterState, FTimerHandle> StanceTimers;

	// For managing buff/debuffs' particle
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Buffable", meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterState, class UNiagaraComponent*> StancesParticles;

	// For managing all debuffs(to remove them using cleanse)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Buffable", meta = (AllowPrivateAccess = "true"))
	TArray<FAppliedDebuffInfo> AppliedStatDebuffs;
};
