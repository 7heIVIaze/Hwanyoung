// Copyright 2024. TEAM DAON. All rights reserved.
// This files for managing Dialogue type structures.
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "DialogueType.generated.h"

/**
 * 
 */

 // enum class to represent dialogue event trigger type
UENUM(BlueprintType)
enum class EDialogueEventType : uint8
{
	None UMETA(DisplayName = "None"),							// Default
	Death UMETA(DisplayName = "Death"),							// Death event
	PuzzleStart UMETA(DisplayName = "PuzzleStart"),				// Start the puzzle event
	PuzzleSuccess UMETA(DisplayName = "PuzzleSuccess"),			// Succeed the puzzle event
	PuzzleFailed UMETA(DisplayName = "PuzzleFailed"),			// Failed the puzzle event
	BattleStart UMETA(DisplayName = "BattleStart"),				// Start battle
};

USTRUCT(BlueprintType)
struct FOptionsDialogue
{
	GENERATED_BODY()

public:
#pragma region Variables
	// The line of the option
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DialogueStructer")
	FText OptionText;

	// The row number to move next line
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DialogueStructer")
	FName RowName;

	// The data table which has scripts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DialogueStructer")
	UDataTable* DataTable;
};

USTRUCT(BlueprintType)
struct FDialogueStructure : public FTableRowBase
{
	GENERATED_BODY()

public:
	FDialogueStructure() 
		: DialogueText(FText::FromString(TEXT("")))
		, NextRow(TEXT(""))
		, DataTable(nullptr)
		, bIsDialogueEnded(false)
		, bIsReplyDialogueOpened(false)
		, ReplyOptions(TArray<FOptionsDialogue>())
		, SpeakerPortrait(nullptr)
		, bIsEventTrigger(false)
		, MelodicSFXOneShot(nullptr)
		, EffortBarkline(nullptr)
		, EventTriggerType(EDialogueEventType::None)
	{ }

public:
#pragma region Variables
	// The line of script
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DialogueStructer")
	FText DialogueText;

	// The row number to move next line
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DialogueStructer")
	FName NextRow;

	// The data table which has scripts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DialogueStructer")
	UDataTable* DataTable;

	// Is the last line of the script
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DialogueStructer")
	bool bIsDialogueEnded;

	// Should reply options be shown?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DialogueStructer")
	bool bIsReplyDialogueOpened;

	// Reply options
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DialogueStructer")
	TArray<FOptionsDialogue> ReplyOptions;

	// The portrait of speaker
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DialogueStructer")
	UTexture2D* SpeakerPortrait;

	// Should this line make an event?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DialogueStructer")
	bool bIsEventTrigger;

	// SFX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DialogueStructer")
	class UFMODEvent* MelodicSFXOneShot;

	// SFX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DialogueStructer")
	class UFMODEvent* EffortBarkline;

	// Event trigger type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DialogueStructer")
	EDialogueEventType EventTriggerType;
#pragma endregion
};

UCLASS()
class HWANYOUNG2_API UDialogueType : public UObject
{
	GENERATED_BODY()
	
};
