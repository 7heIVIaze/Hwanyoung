// Copyright 2024. TEAM DAON. All rights reserved.
// This class is for managing structures and enums for combats.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "CombatTypes.generated.h"

// This enum holds all different types of damages that can be done within the game.
UENUM(BlueprintType)
enum class EDamageType : uint8
{
	None UMETA(DisplayName = "None"),											// N/A
	CloseCombat UMETA(DisplayName = "CloseCombat"),								// Basically the martial arts types of attack
	NormalMeleeSlash UMETA(DisplayName = "NormalMeleeSlash"),					// Melee attack (sword, clubs, whatnot)
	SpiritMeleeSlash UMETA(DisplayName = "SpiritMeleeSlash"),					// Spirit type melee slash attack
	Projectile UMETA(DisplayName = "Projectile"),								// Bow + Arrows, Magical projectile attack, or smth like that
	Parry UMETA(DisplayName = "Parry"),											// Parry counter attack
	Drown UMETA(DisplayName = "Drown"),											// Drowning status effect; this is DOT + slow debuff
	Burn UMETA(DisplayName = "Burn"),											// Burning status effect; DOT
	Poison UMETA(DisplayName = "Poison"),										// Poison effect
	Stun UMETA(DisplayName = "Stun"),											// Stunned effect with SFX
	Bleed UMETA(DisplayName = "Bleed"),											// Bleeding effect
	HealthDrain UMETA(DisplayName = "HealthDrain"),								// HP Drain
	Fear UMETA(DisplayName = "Fear"),											// Fear status effect
	ElectricShock UMETA(DisplayName = "ElectricShock"),							// Thunder hit
	Freeze UMETA(DisplayName = "Freeze"),										// Freeze effect (Mel's cry of freeze); rooted
	HaesolArrowSkill UMETA(DisplayName = "HaesolArrowSkill"),					// Haesol's arrow skill(AOE)
	AOE UMETA(DisplayName = "AOE"),												// AOE skill type attack
	Natural UMETA(DisplayName = "Falling/Natural"),								// Falling Damage or any natural effect damage
};

// This enum holds all the different types of hit reactions that can be elicited by damage.
UENUM(BlueprintType)
enum class EDamageReactionType : uint8
{
	None UMETA(DisplayName = "None"),											// N/A
	CloseCombatHitReaction UMETA(DisplayName = "CloseCombatHitReaction"),		// Close combat attack hit reaction
	MeleeHitReaction UMETA(DisplayName = "MeleeHitReaction"),					// Melee attack hit reaction
	ProjectileHitReaction UMETA(DisplayName = "ProjectileHitReaction"),			// Projectile hit reaction
	EnvironmentalHitReaction UMETA(DisplayName = "EnvironmentalHitReaction"),	// Thunder, falling damage etc hit reaction
	MagicHitReaction UMETA(DisplayName = "MagicHitReaction"),					// Magic attack hit reaction
	DOTHitReaction UMETA(DisplayName = "DOTHitReaction"),						// DOT damage hit reaction
	Stagger UMETA(DisplayName = "Stagger"),										// Stagger effect reaction
	Stunned UMETA(DisplayName = "Stunned"),										// Stun effect reaction
	KnockedBack UMETA(DisplayName = "KnockedBack"),								// KnockedBack effect reaction
	Airborned UMETA(DisplayName = "Airborned"),									// Airborne effect reaction
	Feared UMETA(DisplayName = "Feared"),										// Fear effect reaction
	Rooted UMETA(DisplayName = "Rooted"),										// Rooted effect reaction
};

// struct for managing combat info
USTRUCT(Atomic, BlueprintType)
struct FAttributeSetInfo
{
	GENERATED_BODY()

public:
	// Physical Damage Amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PhysicalDamage;

	// Critical Rate
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CriticalRate;

	// Critical Damage Amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CriticalDamage;

	// Defense Amount to decrease damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Defense;

public:
	FAttributeSetInfo()
		: PhysicalDamage(0.0f)
		, CriticalRate(0.0f)
		, CriticalDamage(0.0f)
		, Defense(0.0f)
	{

	}

	FAttributeSetInfo(float _PhysicalDamage, float _CriticalRate, float _CriticalDamage, float _Defense)
		: PhysicalDamage(_PhysicalDamage)
		, CriticalRate(_CriticalRate)
		, CriticalDamage(_CriticalDamage)
		, Defense(_Defense)
	{ }

	FAttributeSetInfo operator+(const FAttributeSetInfo other) const
	{
		return { PhysicalDamage + other.PhysicalDamage
				, CriticalRate + other.CriticalRate
				, CriticalDamage + other.CriticalDamage
				, Defense + other.Defense };
	}

	FAttributeSetInfo operator-(const FAttributeSetInfo other) const
	{
		return { PhysicalDamage - other.PhysicalDamage
				, CriticalRate - other.CriticalRate
				, CriticalDamage - other.CriticalDamage
				, Defense - other.Defense };
	}
};

// struct for managing CC info(duration, distance(knocked back), height(airborne) etc.)
USTRUCT(Atomic, BlueprintType)
struct FCrowdControlInfo
{
	GENERATED_BODY()

public:
	// CC duration
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration;

	// Knocked back distance
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector KnockedbackDistance;

	// Airborne height
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AirborneHeight;

public:
	FCrowdControlInfo()
		: Duration(0.0f)
		, KnockedbackDistance(FVector(0.0f, 0.0f, 0.0f))
		, AirborneHeight(0.0f)
	{ }

	FCrowdControlInfo(float _Duration, FVector _KnockedbackDistance, float _AirborneHeight)
		: Duration(_Duration)
		, KnockedbackDistance(_KnockedbackDistance)
		, AirborneHeight(_AirborneHeight)
	{ }
};

// struct for managing stat buff info
USTRUCT(Atomic, BlueprintType)
struct FStatBuffInfo
{
	GENERATED_BODY()

public:
	// Attribute set info
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttributeSetInfo AttributeSet;

	// Is this buff/debuff applied by factor
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsFactorBased;

public:
	FStatBuffInfo()
		: AttributeSet()
		, bIsFactorBased(false)
	{}

	FStatBuffInfo(FAttributeSetInfo AttributeSetInfo, bool IsFactorBased)
		: AttributeSet(AttributeSetInfo)
		, bIsFactorBased(IsFactorBased)
	{}

	FStatBuffInfo(float PhysicalDamageFactor, float CriticalRateFactor, float CriticalDamageFactor, float DefenseFactor, bool IsFactorBased)
		: AttributeSet(PhysicalDamageFactor, CriticalRateFactor, CriticalDamageFactor, DefenseFactor)
		, bIsFactorBased(IsFactorBased)
	{}
};

// struct for managing damage info
USTRUCT(Atomic, BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()

public:
	// Damage Amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageAmount;

	// Damage type
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDamageType DamageType;

	// Damage reaction type
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDamageReactionType DamageReactionType;

	// Crowd control info
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCrowdControlInfo CrowdControl;

	// Can this damage deal to invincible
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldDamageInvincible;

	// Can this damage be blocked
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeBlocked;

	// Can this damage be parried
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeParried;

	// Can this damage be forced to interrupt
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldForceInterrupt;

public:
	FDamageInfo()
		: DamageAmount(0.0f)
		, DamageType(EDamageType::None)
		, DamageReactionType(EDamageReactionType::None)
		, CrowdControl(FCrowdControlInfo())
		, bShouldDamageInvincible(false)
		, bCanBeBlocked(false)
		, bCanBeParried(false)
		, bShouldForceInterrupt(false)
	{ }

	FDamageInfo(float _DamageAmount, EDamageType _DamageType, EDamageReactionType _DamageReactionType, FCrowdControlInfo _CrowdControl,
		bool _ShouldDamageInvincible, bool _CanBeBlocked, bool _CanBeParried, bool _ShouldForceInterrupt)
		: DamageAmount(_DamageAmount)
		, DamageType(_DamageType)
		, DamageReactionType(_DamageReactionType)
		, CrowdControl(_CrowdControl)
		, bShouldDamageInvincible(_ShouldDamageInvincible)
		, bCanBeBlocked(_CanBeBlocked)
		, bCanBeParried(_CanBeParried)
		, bShouldForceInterrupt(_ShouldForceInterrupt)
	{ }
};


// struct for managing skill info
USTRUCT(Atomic, BlueprintType)
struct FSkillInfo
{
	GENERATED_BODY()

public:
	// Can this skill be cast?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeCast;

	// Base cooldown time of this skill
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseCooldownTime;

	// Cast lock time
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CastLockTime;

	// Was this skill cast by player?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bWasCastByPlayer;

public:
	FSkillInfo()
		: bCanBeCast(true)
		, BaseCooldownTime(0.0f)
		, CastLockTime(1.0f)
		, bWasCastByPlayer(false)
	{	}

	FSkillInfo(bool _CanBeCast, float _BaseCooldownTime, float _CastLockTime, bool _WasCastByPlayer)
		: bCanBeCast(_CanBeCast)
		, BaseCooldownTime(_BaseCooldownTime)
		, CastLockTime(_CastLockTime)
		, bWasCastByPlayer(_WasCastByPlayer)
	{	}
};

// Skill data for managing skill
UENUM(BlueprintType)
enum class ESkillType : uint8
{
	ESkill,
	QSkill,
	CSkill,
	Ultimate,
	MAX
};

// Aggro NPC's default attack mode
UENUM(BlueprintType)
enum class EDefaultAttackMode : uint8
{
	None,
	Melee,
	Projectile,
	AOE,
	Spawn,
};
// Dummy Actor
UCLASS()
class HWANYOUNG2_API ACombatTypes : public AActor
{
	GENERATED_BODY()
};
