// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HYHPSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageBlocked, bool, _CanBeParried, AActor*, _DamageInstigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnDamageTaken, EDamageReactionType, _DamageResponse, FCrowdControlInfo, _CrowdControl, AActor*, _DamageInstigator, const FHitResult&, _OutHit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealed);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HWANYOUNG2_API UHYHPSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHYHPSystem();

#pragma region Event Graphs
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Called when the owner takes DOT damage.
	UFUNCTION(BlueprintNativeEvent)
	void StartDOT(FDamageInfo _DamageInfo, AActor* _DamageInstigator, float _DamageInterval);

	virtual void StartDOT_Implementation(FDamageInfo _DamageInfo, AActor* _DamageInstigator, float _DamageInterval);

	// Really take DOT damage logic.
	UFUNCTION(BlueprintNativeEvent)
	void DamageOverTime();

	virtual void DamageOverTime_Implementation();

	// Stop DOT Damage.
	UFUNCTION(BlueprintNativeEvent)
	void StopDOT();

	virtual void StopDOT_Implementation();
#pragma endregion

#pragma region Functions
	// Heal Current HP
	UFUNCTION(BlueprintCallable)
	float Heal(float _AmountToHeal);

	// Take damage logic. Call 3 delegate events for each condition.
	UFUNCTION(BlueprintCallable)
	bool TakeDamage(FDamageInfo _DamageInfo, AActor* _DamageInstigator, const FHitResult& _Hit);

	// Consume HP
	UFUNCTION(BlueprintCallable)
	float UseHP(float _AmountToUse);

	// Attack token reserved by AggroMobs for group battle.
	UFUNCTION(BlueprintCallable)
	bool ReserveAttackToken(int _Amount);

	// Return Attack token when AggroMobs finished attack.
	UFUNCTION(BlueprintCallable)
	void ReturnAttackToken(int _Amount);

#pragma region Buffs
	// Apply HP buff
	UFUNCTION(BlueprintCallable, Category = "Buff")
	bool Buff(float _AmountToBuffed);
	
	// Remove HP buff
	UFUNCTION(BlueprintCallable, Category = "Buff")
	bool RemoveBuff(float _AmountToBuffed);

#pragma endregion

#pragma endregion

#pragma region Setter/Getter
	UFUNCTION(BlueprintCallable)
	bool GetIsDead() { return bIsDead; }

	UFUNCTION(BlueprintCallable)
	void SetIsDead(bool _IsDead) { bIsDead = _IsDead; }

	UFUNCTION(BlueprintCallable)
	float GetCurrentHP() { return CurrentHP; }

	UFUNCTION(BlueprintCallable)
	void SetCurrentHP(float _CurrentHP) { CurrentHP = _CurrentHP; }

	UFUNCTION(BlueprintCallable)
	float GetMaxHP() { return MaxHP; }

	UFUNCTION(BlueprintCallable)
	void SetMaxHP(float _MaxHP) { MaxHP = _MaxHP; }

	UFUNCTION(BlueprintCallable)
	bool GetIsInvincible() { return bIsInvincible; }

	UFUNCTION(BlueprintCallable)
	void SetIsInvincible(bool _IsInvincible) { bIsInvincible = _IsInvincible; }

	UFUNCTION(BlueprintCallable)
	bool GetIsBlocking() { return bIsBlocking; }

	UFUNCTION(BlueprintCallable)
	void SetIsBlocking(bool _IsBlocking) { bIsBlocking = _IsBlocking; }

	UFUNCTION(BlueprintCallable)
	bool GetIsInterruptible() { return bIsInterruptible; }

	UFUNCTION(BlueprintCallable)
	void SetIsInterruptible(bool _IsInterruptible) { bIsInterruptible = _IsInterruptible; }

	UFUNCTION(BlueprintCallable)
	bool GetIsParrying() { return bIsParrying; }

	UFUNCTION(BlueprintCallable)
	void SetIsParrying(bool _IsParrying) { bIsParrying = _IsParrying; }

	UFUNCTION(BlueprintCallable)
	int GetAttackTokensCount() { return AttackTokensCount; }

	UFUNCTION(BlueprintCallable)
	void SetMaxAttackTokensCount(int _InitAttackTokenCount) { MaxAttackTokensCount = _InitAttackTokenCount; }

	UFUNCTION(BlueprintCallable)
	int GetMaxAttackTokensCount() { return MaxAttackTokensCount; }

#pragma endregion

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP", meta = (AllowPrivateAccess = "true"))
	float CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP", meta = (AllowPrivateAccess = "true"))
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State", meta = (AllowPrivateAccess = "true"))
	bool bIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State", meta = (AllowPrivateAccess = "true"))
	bool bIsInvincible;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State", meta = (AllowPrivateAccess = "true"))
	bool bIsBlocking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State", meta = (AllowPrivateAccess = "true"))
	bool bIsParrying;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State", meta = (AllowPrivateAccess = "true"))
	bool bIsInterruptible;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Group Battle", meta = (AllowPrivateAccess = "true"))
	int AttackTokensCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Group Battle", meta = (AllowPrivateAccess = "true"))
	int MaxAttackTokensCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP", meta = (AllowPrivateAccess = "true"))
	FTimerHandle DOTTimer;

public:
	// Death Event Dispatcher
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnDeathEvent OnDeath;

	// Damage Blocked Event Dispatcher
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnDamageBlocked OnDamageBlocked;

	// Damage Taken Event Dispatcher
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnDamageTaken OnDamageTaken;

	// Heal Event Dispatcher
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnHealed OnHeal;
};
