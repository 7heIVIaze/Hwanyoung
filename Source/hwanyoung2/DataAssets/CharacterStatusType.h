// Copyright 2024. TEAM DAON. All rights reserved.
// This class is for managing structures and enums for character status.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatTypes.h"
#include "CharacterStatusType.generated.h"

// enum class to represent the buffable category
UENUM(BlueprintType)
enum class EBuffableCategory : uint8
{
	None UMETA(DisplayName = "None"),							// Default none(NULL)
	Stat UMETA(DisplayName = "Stat"),							// Applied to character's stat(movement, DEF, DMG, CRIT etc.)
	CrowdControl UMETA(DisplayName = "CrowdControl"),			// Crowd control(stunned, blind, silenced etc.)
};

// enum class to represent the character's status
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ECharacterState
{
	None = 0 UMETA(DisplayName = "None"),							// normal states
	Vulnerable = 1 << 0 UMETA(DisplayName = "Vulnerable"),			// Decreased Defense(Take twice damage)
	DefenseUp = 1 << 1 UMETA(DisplayName = "DefenseUp"),			// Increased Defence(Take half damage)
	Slowed = 1 << 2 UMETA(DisplayName = "Slowed"),					// Slowed(movement speed down divided)
	Dizzy = 1 << 3 UMETA(DisplayName = "Dizzy"),					// Dizzy(camere shake smoothly. like nausea of minecraft)
	Stunned = 1 << 4 UMETA(DisplayName = "Stunned"),				// Stunned
	Empower = 1 << 5 UMETA(DisplayName = "Empower"),				// Increased attack DMG(plus higher damage critical)
	Weakened = 1 << 6 UMETA(DisplayName = "Weakened"),				// Decreased attack DMG(minus damage critical)
	HPBuff = 1 << 7 UMETA(DisplayName = "HPBuff"),					// Increased maximum hp
	MPBuff = 1 << 8 UMETA(DisplayName = "MPBuff"),					// Increased maximum mp
	Silence = 1 << 9 UMETA(DisplayName = "Silence"),				// Slienced(Do not use skills)
	Blind = 1 << 10 UMETA(DisplayName = "Blind"),					// Blind(miss melee/projectile attack)
};
ENUM_CLASS_FLAGS(ECharacterState);

// enum class to represent the non player character's movement speed
UENUM(BlueprintType)
enum class ENPCMovementSpeed : uint8
{
	Idle UMETA(DisplayName = "Idle"),					// idle
	Walking UMETA(DisplayName = "Walking"),				// walking
	Jogging UMETA(DisplayName = "Jogging"),				// investigating
	Sprinting UMETA(DisplayName = "Sprinting"),			// sprinting
};

// enum class to represent the non player character's behavior state
UENUM(BlueprintType)
enum class ENPCBehaviorState : uint8
{
	Dead UMETA(DisplayName = "Dead"),								// dead
	Passive UMETA(DisplayName = "Passive"),							// passive(hold on)
	Investigating UMETA(DisplayName = "Investigating"),				// investigating
	Seen UMETA(DisplayName = "Seen"),								// watched player
	Combat UMETA(DisplayName = "Combat"),							// start to combat, Only available for AggroNPCs.
	Frozen UMETA(DisplayName = "Frozen"),							// stunned, etc.
	Fleeing UMETA(DisplayName = "Fleeing"),							// fleeing, Only available for Puzzle NPCs.
};

// struct to manage buffable info
USTRUCT(BlueprintType)
struct FBuffableInfo
{
	GENERATED_BODY()

public:
	// Buff/Debuff category(stat/cc)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBuffableCategory BuffableCategory;

	// Buff/Debuff type
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterState BuffableType;
	
	// the duration of buff/debuff
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration;

	// CC info
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCrowdControlInfo CrowdControlInfo;

	// Stat buff/debuff info
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FStatBuffInfo StatBuffInfo;

	// the amount of stat(HP/MP) buff/debuff
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Magnitude;

public:
	FBuffableInfo()
		: BuffableCategory(EBuffableCategory::None)
		, BuffableType(ECharacterState::None)
		, Duration(0.0f)
		, CrowdControlInfo(FCrowdControlInfo())
		, StatBuffInfo(FAttributeSetInfo())
		, Magnitude(0.0f)
	{}

	FBuffableInfo(EBuffableCategory _BuffableCategory, ECharacterState _BuffableType, float _Duration,
					FCrowdControlInfo _CrowdControlInfo, FAttributeSetInfo _StatBuffInfo, float _Magnitude)
		: BuffableCategory(_BuffableCategory)
		, BuffableType(_BuffableType)
		, Duration(_Duration)
		, CrowdControlInfo(_CrowdControlInfo)
		, StatBuffInfo(_StatBuffInfo)
		, Magnitude(_Magnitude)
	{}
};


// struct to manage applied debuff info
USTRUCT(BlueprintType)
struct FAppliedDebuffInfo
{
	GENERATED_BODY()

public:
	// Debuff Info
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBuffableInfo DebuffInfo;

	// The activated timer handle
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle DebuffTimer;

public:
	FAppliedDebuffInfo()
		: DebuffInfo(FBuffableInfo())
		, DebuffTimer(FTimerHandle())
	{}

	FAppliedDebuffInfo(FBuffableInfo _DebuffInfo, FTimerHandle _Timer)
		: DebuffInfo(_DebuffInfo)
		, DebuffTimer(_Timer)
	{}
};
UCLASS()
class HWANYOUNG2_API ACharacterStatusType : public AActor
{
	GENERATED_BODY()

};
