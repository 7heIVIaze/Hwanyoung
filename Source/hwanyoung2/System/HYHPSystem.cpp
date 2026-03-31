// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYHPSystem.h"
#include "DataAssets/CombatTypes.h"

// Sets default values for this component's properties
UHYHPSystem::UHYHPSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UHYHPSystem::BeginPlay()
{
	Super::BeginPlay();

	AttackTokensCount = MaxAttackTokensCount;
	// ...
	
}


// Called every frame
void UHYHPSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UHYHPSystem::Heal(float AmountToHeal)
{
	// If the character is dead, escape
	if (bIsDead)
	{
		return 0.0f;
	}

	// Add healing amount to current HP 
	CurrentHP = FMath::Clamp(CurrentHP + AmountToHeal, 0.0f, MaxHP);
	OnHeal.Broadcast();

	return CurrentHP;
}

bool UHYHPSystem::TakeDamage(FDamageInfo DamageInfo, AActor* DamageInstigator, const FHitResult& _Hit)
{
	// Is the damage able to hurt the invincible character or character is not invincible?
	bool bBeDamageAbleToDeal = !bIsDead && (DamageInfo.bShouldDamageInvincible || !bIsInvincible);

	// Can this damage be blocked and is character blocking?
	bool bIsDamageBlocked = !bIsBlocking && DamageInfo.bCanBeBlocked;

	if (bBeDamageAbleToDeal)
	{
		// Block Damage
		if (bIsDamageBlocked)
		{
			// Broadcast that blocked damage. Whether or not there is a pairing will be determined in the bound function.
			OnDamageBlocked.Broadcast(DamageInfo.bCanBeParried, DamageInstigator);

			return false;
		}

		// Do Damage
		else
		{
			// Calculate damage logic current HP
			CurrentHP = FMath::Clamp(CurrentHP - DamageInfo.DamageAmount, 0.0f, MaxHP);

			// if current HP is less equal to 0, dead
			if (CurrentHP <= 0.0f)
			{
				bIsDead = true;

				OnDeath.Broadcast(DamageInstigator);

				return true;
			}

			// Check can be interruptable
			if (bIsInterruptible || DamageInfo.bShouldForceInterrupt)
			{
				// Do Take Damage logic
				OnDamageTaken.Broadcast(DamageInfo.DamageReactionType, DamageInfo.CrowdControl, DamageInstigator, _Hit);
			}

			return true;
		}
	}
	// No Damage
	else
	{
		// Do Nothing
		return false;
	}

	// Failed damage
	return false;
}

float UHYHPSystem::UseHP(float AmountToUse)
{
	CurrentHP = FMath::Clamp(CurrentHP - AmountToUse, 0.0f, MaxHP);

	return CurrentHP;
}

bool UHYHPSystem::ReserveAttackToken(int Amount)
{
	// Check if a mob can get attack token
	if (AttackTokensCount >= Amount)
	{
		// Remove one token and add it to notify that there is a mob currently attacking.
		AttackTokensCount = FMath::Clamp(AttackTokensCount - Amount, 0, MaxAttackTokensCount);

		return true;
	}
	
	return false;
}

void UHYHPSystem::ReturnAttackToken(int Amount)
{
	AttackTokensCount = FMath::Clamp(AttackTokensCount + Amount, 0, MaxAttackTokensCount);

	return;
}

void UHYHPSystem::StartDOT(FDamageInfo DamageInfo, AActor* DamageInstigator, float DamageInterval)
{

	// Is the damage able to hurt the invincible character or character is not invincible?
	bool bBeDamageAbleToDeal = !bIsDead && (DamageInfo.bShouldDamageInvincible || !bIsInvincible);

	// Can this damage be blocked and is character blocking?
	bool bIsDamageBlocked = !bIsBlocking && DamageInfo.bCanBeBlocked;

	if (true == bBeDamageAbleToDeal)
	{
		// if character taken damage
		if (false == bIsDamageBlocked)
		{
			GetWorld()->GetTimerManager().SetTimer(DOTTimer, FTimerDelegate::CreateLambda([this, DamageInfo, DamageInstigator]()
			{
				// Damage Over Time
				this->CurrentHP = FMath::Clamp(this->CurrentHP - DamageInfo.DamageAmount, 0.0f, this->MaxHP);

				// If the character is dead.
				if (this->CurrentHP <= 0)
				{
					this->bIsDead = true;
					this->OnDeath.Broadcast(DamageInstigator);

					StopDOT();
				}
				else
				{
					if (this->bIsInterruptible || DamageInfo.bShouldForceInterrupt)
					{
						this->OnDamageTaken.Broadcast(DamageInfo.DamageReactionType, DamageInfo.CrowdControl, DamageInstigator, FHitResult());
					}
				}


			}), DamageInterval, true);
		}
	}
}

void UHYHPSystem::DamageOverTime()
{
	return;
}

void UHYHPSystem::StopDOT()
{
	GetWorld()->GetTimerManager().ClearTimer(DOTTimer);
}

bool UHYHPSystem::Buff(float AmountToBuffed)
{
	// If the character is not dead, then apply buff
	if (false == bIsDead)
	{
		// Increase max HP temporarily.
		MaxHP += AmountToBuffed;

		// And increase(heal) current HP.(that is permanant)
		Heal(AmountToBuffed);

		return true;
	}

	return false;
}

bool UHYHPSystem::RemoveBuff(float AmountToBuffed)
{
	// Decrease the increased amount of HP
	MaxHP -= AmountToBuffed;

	// Adjust the current HP to be between 0 and max hp.
	CurrentHP = FMath::Clamp(CurrentHP, 0.0f, MaxHP);

	return true;
}
