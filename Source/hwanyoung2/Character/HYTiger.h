// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HYBossNPCBase.h"
#include "Components/TimelineComponent.h"
#include "HYTiger.generated.h"

/**
 * 
 */

class UNiagaraSystem;
UCLASS()
class HWANYOUNG2_API AHYTiger : public AHYBossNPCBase
{
	GENERATED_BODY()

public:
	AHYTiger();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

#pragma region Event Graphs

#pragma region Lunge Skill
	// Lunge skill
	UFUNCTION(BlueprintCallable)
	void Lunge(AActor* _AttackTarget);

	virtual void OnMontageEnded(UAnimMontage* _Montage, bool _Interrupted) override;

	// Callback Func while timeline is playing
	UFUNCTION(BlueprintCallable)
	void LungeTimelineUpdate(FVector Location);

	// Callback Func when timeline finished.
	UFUNCTION(BlueprintCallable)
	void LungeTimelineFinished();

	// Lunge Land Event
	UFUNCTION(BlueprintCallable)
	virtual void OnLand(const FHitResult& Hit);
#pragma endregion

#pragma region Crack of the Earth Skill
	// <Carck of the Earth> skill
	UFUNCTION(BlueprintCallable)
	void CrackoftheEarth(AActor* _AttackTarget);

	// Callback Func while timeline is playing
	UFUNCTION(BlueprintCallable)
	void JumpingTimelineUpdate(float LocationZ);

	// Callback Func when timeline finished.
	UFUNCTION(BlueprintCallable)
	void JumpingTimelineFinished();

	// Make trail projectile Event
	UFUNCTION(BlueprintCallable)
	virtual void MakeTrailProjectile();

	// Make trail projectile Event
	UFUNCTION(BlueprintCallable)
	void TrailDamage(AActor* OtherActor, const FHitResult& Hit);
#pragma endregion

#pragma region Ultimate Skill
	// Use ultimate skill
	UFUNCTION(BlueprintCallable)
	void UseUltSkill(AActor* _AttackTarget);

	// Use Roar
	UFUNCTION(BlueprintCallable)
	void Roar(AActor* _AttackTarget);

	// To do when roar notify event has been called.
	UFUNCTION(BlueprintCallable)
	void OnRoarNotify();

	// ultimate jump skill(similar to lunge but different)
	UFUNCTION(BlueprintCallable)
	void UltJump(AActor* _AttackTarget);

	// On Land Event after using ultimate jump
	UFUNCTION(BlueprintCallable)
	virtual void OnUltLand(const FHitResult& Hit);

	// Use spinning slash
	UFUNCTION(BlueprintCallable)
	void SpinningSlash(AActor* _AttackTarget);

	// Callback Func when the spin notify has been called.
	UFUNCTION(BlueprintCallable)
	void OnSpinningNotified();

	// Use Bite combo
	UFUNCTION(BlueprintCallable)
	void Bite(AActor* _AttackTarget);

	// Callback Func when init bite notify has been called.
	UFUNCTION(BlueprintCallable)
	void OnInitBiteNotify();

	// Callback Func when final bite notify has been called.
	UFUNCTION(BlueprintCallable)
	void OnFinalBiteNotify();

	// Tiger bite helper
	UFUNCTION(BlueprintCallable)
	void TigerBite(bool _IsFirstBite);

#pragma endregion

#pragma endregion

#pragma region Variables
	// Tiger lunge animation montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	UAnimMontage* TigerLungeMontage;

	// Tiger roar animation montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* TigerRoarMontage;

	// Tiger ultimate jump animation montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* TigerChargeJumpMontage;

	// Tiger spinning slash animation montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* TigerSpinningSlashMontage;

	// Tiger biting animation montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* TigerBitingMontage;

	// Tiger biting trail effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UNiagaraSystem* NS_Trail;

	// Tiger biting dash trail effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UNiagaraSystem* NS_DashTrail;

	// player character(AttackTarget) before using skill.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Skill")
	AActor* Target;

	// The location before using skill.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Skill")
	FVector CurrentLocation;

	// Lunge Timeline
	UPROPERTY()
	FTimeline LungeTimeline;

	// Lunge timeline curve vector
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill")
	UCurveVector* LungeCurveVector;

	// Lunge AOE actor class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill")
	TSubclassOf<AActor> LungeAOEClass;

	// Ultimate jump AOE actor class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	TSubclassOf<AActor> UltJumpAOEClass;

	// Spinning AOE actor class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	TSubclassOf<AActor> SpinningSlashAOEClass;

	// Jumping Timeline
	UPROPERTY()
	FTimeline JumpTimeline;

	// Jump timeline curve float
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	UCurveFloat* JumpCurveFloat;

	// Trail projectile actor class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	TSubclassOf<AActor> EarthTrailClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Skill")
	int CrackUseCount = 0;

	// Has activated to use ultimate skill
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	bool bHasActivateUlt;

	// The probability to make player character's state bleeding after melee attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float SlashBleedProbability;

	// Damage factor to increase slash damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float DamageFactor;

#pragma endregion
	
};
