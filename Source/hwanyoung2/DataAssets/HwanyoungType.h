// Copyright 2024. TEAM DAON. All rights reserved.
// This class is for managing structures and enums for character status.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "HwanyoungType.generated.h"

enum class ESaveApplyState
{
	None,
	Skipped,
	Changed,
	Failed,
};

USTRUCT(BlueprintType)
struct FAffinityLevelUpData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FAffinityLevelUpData()
		: AffinityPoint(0)
	{
	}

	// the point to move next affinity level.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affinity")
	int32 AffinityPoint;
};

USTRUCT(BlueprintType)
struct FAffinityData
{
	GENERATED_USTRUCT_BODY()

public:
	FAffinityData()
		: AffinityLevel(0)
		, AffinityPoint(0)
	{
	}

	// Current affinity level.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affinity")
	int32 AffinityLevel;

	// The point of current affinity level.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affinity")
	int32 AffinityPoint;
};

// Boss type for boss hp bar widget
USTRUCT(BlueprintType)
struct FBossType : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	// Boss NPC's name .
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	FText BossName;

	// The title of boss npc.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	FText BossTitle;

	// the color of boss name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	FColor BossNameColor;
};