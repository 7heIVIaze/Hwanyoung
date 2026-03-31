// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HYHPSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathEvent, AActor*, DamageInstigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageBlocked, bool, CanBeParried, AActor*, DamageInstigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnDamageTaken, EDamageReactionType, DamageResponse, FCrowdControlInfo, CrowdControl, AActor*, DamageInstigator, const FHitResult&, OutHit);
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
	UFUNCTION(BlueprintCallable)
	void StartDOT(FDamageInfo DamageInfo, AActor* DamageInstigator, float DamageInterval);

	// Really take DOT damage logic.
	UFUNCTION(BlueprintCallable)
	void DamageOverTime();

	// Stop DOT Damage.
	UFUNCTION(BlueprintCallable)
	void StopDOT();

#pragma endregion

#pragma region Functions
	// Heal Current HP
	UFUNCTION(BlueprintCallable)
	float Heal(float AmountToHeal);

	// Take damage logic. Call 3 delegate events for each condition.
	UFUNCTION(BlueprintCallable)
	bool TakeDamage(FDamageInfo DamageInfo, AActor* DamageInstigator, const FHitResult& Hit);

	// Consume HP
	UFUNCTION(BlueprintCallable)
	float UseHP(float AmountToUse);

	// Attack token reserved by AggroMobs for group battle.
	UFUNCTION(BlueprintCallable)
	bool ReserveAttackToken(int Amount);

	// Return Attack token when AggroMobs finished attack.
	UFUNCTION(BlueprintCallable)
	void ReturnAttackToken(int Amount);

#pragma region Buffs
	// Apply HP buff
	UFUNCTION(BlueprintCallable, Category = "Buff")
	bool Buff(float AmountToBuffed);
	
	// Remove HP buff
	UFUNCTION(BlueprintCallable, Category = "Buff")
	bool RemoveBuff(float AmountToBuffed);

#pragma endregion

#pragma endregion

#pragma region Setter/Getter
	UFUNCTION(BlueprintCallable)
	bool GetIsDead() { return bIsDead; }

	UFUNCTION(BlueprintCallable)
	void SetIsDead(bool IsDead) { bIsDead = IsDead; }

	UFUNCTION(BlueprintCallable)
	float GetCurrentHP() { return CurrentHP; }

	UFUNCTION(BlueprintCallable)
	void SetCurrentHP(float NewHP) { CurrentHP = NewHP; }

	UFUNCTION(BlueprintCallable)
	float GetMaxHP() { return MaxHP; }

	UFUNCTION(BlueprintCallable)
	void SetMaxHP(float NewHP) { MaxHP = NewHP; }

	UFUNCTION(BlueprintCallable)
	bool GetIsInvincible() { return bIsInvincible; }

	UFUNCTION(BlueprintCallable)
	void SetIsInvincible(bool IsInvincible) { bIsInvincible = IsInvincible; }

	UFUNCTION(BlueprintCallable)
	bool GetIsBlocking() { return bIsBlocking; }

	UFUNCTION(BlueprintCallable)
	void SetIsBlocking(bool IsBlocking) { bIsBlocking = IsBlocking; }

	UFUNCTION(BlueprintCallable)
	bool GetIsInterruptible() { return bIsInterruptible; }

	UFUNCTION(BlueprintCallable)
	void SetIsInterruptible(bool IsInterruptible) { bIsInterruptible = IsInterruptible; }

	UFUNCTION(BlueprintCallable)
	bool GetIsParrying() { return bIsParrying; }

	UFUNCTION(BlueprintCallable)
	void SetIsParrying(bool IsParrying) { bIsParrying = IsParrying; }

	UFUNCTION(BlueprintCallable)
	int GetAttackTokensCount() { return AttackTokensCount; }

	UFUNCTION(BlueprintCallable)
	void SetMaxAttackTokensCount(int InitAttackTokenCount) { MaxAttackTokensCount = InitAttackTokenCount; }

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
