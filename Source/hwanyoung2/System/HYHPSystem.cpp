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

float UHYHPSystem::Heal(float _AmountToHeal)
{
	// If the character is dead, escape
	if (bIsDead)
	{
		return 0.0f;
	}

	// Add healing amount to current HP 
	CurrentHP = FMath::Clamp(CurrentHP + _AmountToHeal, 0.0f, MaxHP);
	OnHeal.Broadcast();

	return CurrentHP;
}

bool UHYHPSystem::TakeDamage(FDamageInfo _DamageInfo, AActor* _DamageInstigator, const FHitResult& _Hit)
{
	// Is the damage able to hurt the invincible character or character is not invincible?
	bool bBeDamageAbleToDeal = !bIsDead && (_DamageInfo.bShouldDamageInvincible || !bIsInvincible);

	// Can this damage be blocked and is character blocking?
	bool bIsDamageBlocked = !bIsBlocking && _DamageInfo.bCanBeBlocked;

	if (bBeDamageAbleToDeal)
	{
		// Block Damage
		if (bIsDamageBlocked)
		{
			// Broadcast that blocked damage. Whether or not there is a pairing will be determined in the bound function.
			OnDamageBlocked.Broadcast(_DamageInfo.bCanBeParried, _DamageInstigator);

			return false;
		}

		// Do Damage
		else
		{
			// Calculate damage logic current HP
			CurrentHP = FMath::Clamp(CurrentHP - _DamageInfo.DamageAmount, 0.0f, MaxHP);

			// if current HP is less equal to 0, dead
			if (CurrentHP <= 0.0f)
			{
				bIsDead = true;

				OnDeath.Broadcast();

				return true;
			}

			// Check can be interruptable
			if (bIsInterruptible || _DamageInfo.bShouldForceInterrupt)
			{
				// Do Take Damage logic
				OnDamageTaken.Broadcast(_DamageInfo.DamageReactionType, _DamageInfo.CrowdControl, _DamageInstigator, _Hit);
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

float UHYHPSystem::UseHP(float _AmountToUse)
{
	CurrentHP = FMath::Clamp(CurrentHP - _AmountToUse, 0.0f, MaxHP);

	return CurrentHP;
}

bool UHYHPSystem::ReserveAttackToken(int _Amount)
{
	// Check if a mob can get attack token
	if (AttackTokensCount >= _Amount)
	{
		// Remove one token and add it to notify that there is a mob currently attacking.
		AttackTokensCount = FMath::Clamp(AttackTokensCount - _Amount, 0, MaxAttackTokensCount);

		return true;
	}
	
	return false;
}

void UHYHPSystem::ReturnAttackToken(int _Amount)
{
	AttackTokensCount = FMath::Clamp(AttackTokensCount + _Amount, 0, MaxAttackTokensCount);

	return;
}

void UHYHPSystem::StartDOT_Implementation(FDamageInfo _DamageInfo, AActor* _DamageInstigator, float _DamageInterval)
{

	// Is the damage able to hurt the invincible character or character is not invincible?
	bool bBeDamageAbleToDeal = !bIsDead && (_DamageInfo.bShouldDamageInvincible || !bIsInvincible);

	// Can this damage be blocked and is character blocking?
	bool bIsDamageBlocked = !bIsBlocking && _DamageInfo.bCanBeBlocked;

	if (true == bBeDamageAbleToDeal)
	{
		// if character taken damage
		if (false == bIsDamageBlocked)
		{
			GetWorld()->GetTimerManager().SetTimer(DOTTimer, FTimerDelegate::CreateLambda([this, _DamageInfo, _DamageInstigator]()
			{
				// Damage Over Time
				this->CurrentHP = FMath::Clamp(this->CurrentHP - _DamageInfo.DamageAmount, 0.0f, this->MaxHP);

				// If the character is dead.
				if (this->CurrentHP <= 0)
				{
					this->bIsDead = true;
					this->OnDeath.Broadcast();

					StopDOT_Implementation();
				}
				else
				{
					if (this->bIsInterruptible || _DamageInfo.bShouldForceInterrupt)
					{
						this->OnDamageTaken.Broadcast(_DamageInfo.DamageReactionType, _DamageInfo.CrowdControl, _DamageInstigator, FHitResult());
					}
				}


			}), _DamageInterval, true);
		}
	}
}

void UHYHPSystem::DamageOverTime_Implementation()
{
	return;
}

void UHYHPSystem::StopDOT_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(DOTTimer);
}

bool UHYHPSystem::Buff(float _AmountToBuffed)
{
	// If the character is not dead, then apply buff
	if (false == bIsDead)
	{
		// Increase max HP temporarily.
		MaxHP += _AmountToBuffed;

		// And increase(heal) current HP.(that is permanant)
		Heal(_AmountToBuffed);

		return true;
	}

	return false;
}

bool UHYHPSystem::RemoveBuff(float _AmountToBuffed)
{
	// Decrease the increased amount of HP
	MaxHP -= _AmountToBuffed;

	// Adjust the current HP to be between 0 and max hp.
	CurrentHP = FMath::Clamp(CurrentHP, 0.0f, MaxHP);

	return true;
}
