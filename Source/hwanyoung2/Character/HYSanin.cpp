// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYSanin.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIPlayer/HYAIController.h"
#include "Weapons/Projectiles/HYGonggiProjectile.h"
#include "Interfaces/Damagable.h"

void AHYSanin::GonggiNori_Implementation()
{
	// If the sanin's stack is over than 3
	if (EnableRockShoot >= 3)
	{
		
		// Fire one shot to target
		ShootGonggiStone();
	}

	// If the combo stack is over than 2
	if (EnableRockShoot > 2)
	{
		// Fire two shots like shot gun
		ShootGonggiStone(30.0f, false);
		ShootGonggiStone(-30.0f, false);
	}

	FTimerHandle ShootDelayTimer;

	// Delay the last shot.
	GetWorldTimerManager().SetTimer(ShootDelayTimer, FTimerDelegate::CreateLambda([&]() {
		switch (EnableRockShoot)
		{
			// If current gonngi stack is 2 or 4, then shoot one rock again.
			case 2:
			case 4:
			{
				ShootGonggiStone();
			}
		}
	}), 0.5f, false);
}

void AHYSanin::ShootGonggiStone_Implementation(float _Radian = 0.0f, bool _FiredAtTarget = true)
{
	FVector SpawnLocation = GetMesh()->GetSocketLocation(TEXT("hand_l")) + FVector(_Radian, 0.0f, 0.0f);

	FVector Forward = GetActorForwardVector();
	FRotator Rotator = FRotator(0.0f, _Radian, 0.0f);

	FRotator SpawnRotator = Rotator.RotateVector(Forward).Rotation();
	// If the attack target is not null and not destroyed
	if (IsValid(AICBase->AttackTarget))
	{
		AHYGonggiProjectile* GonggiProjectile = GetWorld()->SpawnActor<AHYGonggiProjectile>(GonggiClass, 
			FTransform(SpawnRotator, SpawnLocation, FVector(0.0f)), FActorSpawnParameters::FActorSpawnParameters());

		// If the projectile is aimed at the player character
		if (_FiredAtTarget)
		{
			// Rotate to target for firing to player
			GonggiProjectile->RotateToTarget();
		}
		else
		{
			GonggiProjectile->ShootStraight();
		}

		// Bind impact event
		GonggiProjectile->OnProjectileImpact.AddDynamic(this, &AHYSanin::GonggiDamage);
	}
}

void AHYSanin::GonggiDamage(AActor* _OtherActor, const FHitResult& _Hit)
{
	// Make Damage info
	FDamageInfo DamageInfo(17.5f, EDamageType::Projectile, EDamageReactionType::ProjectileHitReaction,
		FCrowdControlInfo::FCrowdControlInfo(), true, true, false, true);

	if (IDamagable* DamagableInterface = Cast<IDamagable>(_OtherActor))
	{
		bool bHasTakeDamage = DamagableInterface->TakeDamage(DamageInfo, this, 0.0f, 0.0f, _Hit);

		// Succeed to deal damage
		if (bHasTakeDamage)
		{
			// Increase the rock stack
			CurrentRockStack = FMath::Clamp(++CurrentRockStack, 1, 35);

			// and each stack makes combo dan increase.
			switch (CurrentRockStack)
			{
				case 7:
				{
					EnableRockShoot = 2;
					break;
				}
				case 14:
				{
					EnableRockShoot = 3;
					break;
				}
				case 28:
				{
					EnableRockShoot = 4;
					break;
				}
			}
		}
	}
}