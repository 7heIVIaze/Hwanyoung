// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/CombatSystem.h"
#include "HYWeaponMechanism.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAimBegin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAimEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDrawOngoing, float, _MaxDrawTime, float, _CurrentDrawTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDrawEnd);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HWANYOUNG2_API UHYWeaponMechanism : public UActorComponent, public ICombatSystem
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHYWeaponMechanism();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region Functions
	// Equip current weapon
	// @param : weapon index = index of weapon to equip 
	UFUNCTION(BlueprintCallable)
	void EquipWeapon(int _WeaponIndex);

	// Unequip current weapon
	UFUNCTION(BlueprintCallable)
	void UnequipWeapon();

	// Spawn projectile for aiming
	UFUNCTION(BlueprintCallable)
	void SpawnArrow();

	// Destroy projectile bc the player change weapon etc.
	UFUNCTION(BlueprintCallable)
	void DestroyArrow();

	// Fire the arrow.
	UFUNCTION(BlueprintCallable)
	void FireAimedArrow();

	// Calculate the direction
	UFUNCTION(BlueprintCallable)
	FVector CalculateAimDirection();

	// Calculate the projectile damage while drawing
	UFUNCTION(BlueprintCallable)
	bool CalculateProjectileDamage(float _Strength, float& OutGeneratedDamage);

	// Trace melee weapon mesh
	UFUNCTION(BlueprintCallable)
	void PerformWeaponTrace();
#pragma endregion

#pragma region Events
	// Aim bow
	UFUNCTION(BlueprintNativeEvent)
	void ProjectileAimBegin();

	virtual void ProjectileAimBegin_Implementation();

	// Aim bow
	UFUNCTION(BlueprintNativeEvent)
	void ProjectileAimEnd();

	virtual void ProjectileAimEnd_Implementation();

	// Fire arrow
	UFUNCTION(BlueprintNativeEvent)
	void FireArrowBegin();

	virtual void FireArrowBegin_Implementation();

	// Fire arrow
	UFUNCTION(BlueprintNativeEvent)
	void FireArrowEnd();

	virtual void FireArrowEnd_Implementation();

	// Draw bow
	UFUNCTION(BlueprintNativeEvent)
	void DrawBegin();

	virtual void DrawBegin_Implementation();

	// Draw bow
	UFUNCTION(BlueprintNativeEvent)
	void DrawEnd();

	virtual void DrawEnd_Implementation();

	// Increment draw time : this function is intended to increase damage while drawing the bow
	UFUNCTION(BlueprintNativeEvent)
	void IncrementDrawTime();

	virtual void IncrementDrawTime_Implementation();

	// A function to bind to the ArrowRef's arrow collision event dispatcher that deals damage to the hit actor
	UFUNCTION(BlueprintNativeEvent)
	void DamageActor(float _Strength, AActor* _HitActor, const FHitResult& _HitResult);

	virtual void DamageActor_Implementation(float _Strength, AActor* _HitActor, const FHitResult& _HitResult);

#pragma endregion

#pragma region Inferfaces
	// Toggle weapon trace.
	UFUNCTION(BlueprintCallable)
	virtual void ToggleWeaponTrace(bool _WeaponTraceOn) override;

	// Get Mesh : currently unused
	UFUNCTION(BlueprintCallable)
	virtual USkeletalMeshComponent* GetCharacterMesh() override { return WeaponRef->GetWeaponMesh(); }
#pragma endregion

#pragma region Setter/Getter
	UFUNCTION(BlueprintCallable)
	AHYWeaponBase* GetWeaponReference() { return WeaponRef; }

	UFUNCTION(BlueprintCallable)
	bool GetIsAimingProjectile() { return bIsAimingProjectile; }
#pragma endregion

private:
	// Is Current Weapon(Slash weapon; sword, clubs, ...) tracing?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism|Slash Trace", meta = (AllowPrivateAccess = "true"))
	bool bWeaponTrace;

	// Location of top socket of weapon.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism|Slash Trace", meta = (AllowPrivateAccess = "true"))
	FVector PreviousTopSocket;

	// Location of bottom socket of weapon.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism|Slash Trace", meta = (AllowPrivateAccess = "true"))
	FVector PreviousBottomSocket;;

	// Sets of already damaged actor.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism|Slash Trace", meta = (AllowPrivateAccess = "true"))
	TSet<AActor*> AlreadyDamaged;

	// Is player aiming to shoot projectile?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism|Projectile", meta = (AllowPrivateAccess = "true"))
	bool bIsAimingProjectile;

	// How many rotated.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism|Projectile", meta = (AllowPrivateAccess = "true"))
	FRotator AimRotationRate;

	// Original rotation rate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism|Projectile", meta = (AllowPrivateAccess = "true"))
	FRotator OriginalRotationRate;

	// Arrow(projectile) Reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism|Projectile", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class AHYPlayerProjectileBase> ArrowRef;

	// The name of the socket from which the projectile will be fired.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism|Projectile", meta = (AllowPrivateAccess = "true"))
	FName ProjectileSocketName;

	// Is the bow drawing?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism|Projectile", meta = (AllowPrivateAccess = "true"))
	bool bIsDrawingBow;

	// Total draw time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism|Projectile", meta = (AllowPrivateAccess = "true"))
	float DrawTime;

	// Drawing increment time per frame
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism|Projectile", meta = (AllowPrivateAccess = "true"))
	float DrawIncrementTime;

	// bow drawing timer
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism|Projectile", meta = (AllowPrivateAccess = "true"))
	FTimerHandle DrawBowTimer;

	// Is the player firing projectile?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism|Projectile", meta = (AllowPrivateAccess = "true"))
	bool bIsFiringArrow;

	// Firing Projectile Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism|Projectile", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> ShootBowMontage;

	// Bow Reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism|Projectile", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class AHYBowBase> BowRef;

	// Weapon class for spawning.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> WeaponClass;

	// Weapon class(close combat) for spawning.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism|Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> CloseCombatWeaponClass;


	// Weapon class(melee combat) for spawning.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism|Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> MeleeCombatWeaponClass;


	// Weapon class(projectile) for spawning.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism|Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> ProjectileWeaponClass;

	// Weapon class for spawning.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism|Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> ProjectileClass;

	// Melee weapon socker name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism", meta = (AllowPrivateAccess = "true"))
	FName WeaponSocketName;

	// Owner reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class AHYPlayerCharacterBase> PlayerRef;

	// Current weapon reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class AHYWeaponBase> WeaponRef;

	// Passive heal amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMechanism", meta = (AllowPrivateAccess = "true"))
	float PassiveHealAmount;

	// Bow Draw SFX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFMODEvent> BowDrawSound;

public:
	// Aim Begin Event Dispatcher
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnAimBegin OnAimBegin;

	// Aim End Event Dispatcher
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnAimEnd OnAimEnd;

	// Draw Ongoing Event Dispatcher;
	// @param : maxDrawTime of bow reference, CurrentDrawTime.
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnDrawOngoing OnDrawOngoing;

	// Draw End Event Dispatcher
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnDrawEnd OnDrawEnd;
};
