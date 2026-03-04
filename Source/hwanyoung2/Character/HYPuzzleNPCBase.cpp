// Copyright 2024. TEAM DAON. All rights reserved.

#include "Character/HYPuzzleNPCBase.h"

#include "DataAssets/CharacterStatusType.h"
#include "DataAssets/DialogueType.h"
#include "HYPlayerCharacterBase.h"
#include "Player/HYPlayerCharacController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"
#include "AIPlayer/HYAIController.h"
#include "HYHPSystem.h"
#include "HYGameInstance.h"
#include "Stats/HYCombatSystem.h"
#include "DialogueableSystem/HYDialogueComponent.h"
#include "Kismet/GameplayStatics.h"
#include "System/HYGroupManagerSubsystem.h"

AHYPuzzleNPCBase::AHYPuzzleNPCBase()
{
	HPSystem = CreateDefaultSubobject<UHYHPSystem>(TEXT("HP System"));
	DialogueSystem = CreateDefaultSubobject<UHYDialogueComponent>(TEXT("Dialogue System"));

	HitVFXPoint = CreateDefaultSubobject<USceneComponent>(TEXT("HitVFXPoint"));
	HitVFXPoint->SetupAttachment(RootComponent);

	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	Widget->SetupAttachment(RootComponent);

	bCanStartPuzzle = true;
}

void AHYPuzzleNPCBase::BeginPlay()
{
	InitPuzzleCharacterStates();
	HPSystem->OnDeath.AddDynamic(this, &AHYPuzzleNPCBase::Death);

	UUserWidget* NPCHealthBarHUD = CreateWidget<UUserWidget>(this, NPCHealthWidget);
	NPCHealthBarHUD->SetDamagableActor(this);
	Widget->SetWidget(NPCHealthBarHUD);
	NPCHealthBarHUD->SetPercentage();
	Widget->SetVisibility(false);

	DialogueSystem->OnDialogueEventTrigger.AddDynamic(this, &AHYPuzzleNPCBase::DialogueEndEvent);


}

void AHYPuzzleNPCBase::Death_Implementation()
{

}

void AHYPuzzleNPCBase::DialogueEndEvent(EDialogueEventType EventTriggerType)
{
	switch (EventTriggerType)
	{
		case EDialogueEventType::PuzzleStart:
		{
			// If puzzle could be started
			if (bCanStartPuzzle)
			{
				// Prevents duplicate execution of puzzle
				if (!bIsPuzzleAlreadyStarted)
				{
					bIsPuzzleAlreadyStarted = true;
					StartPuzzleContent();
				}
			}
			else
			{
				DialogueSystem->bIsDialogueEnded = false;
			}
			break;
		}
		// Puzzle cleared event
		case EDialogueEventType::PuzzleSuccess:
		{
			// If player has already cleared puzzle, then just finish puzzle system
			if (bIsFirstCleared)
			{
				EndPuzzle();
			}
			// If not, then give reward to player
			else
			{
				GiveReward();
			}
			break;
		}
		default:
		{

		}
	}
}

void AHYPuzzleNPCBase::StartPuzzleContent()
{
	bCanStartPuzzle = false;
	// this will be implemented in the child class
}

void AHYPuzzleNPCBase::EndPuzzle()
{
	bCanStartPuzzle = true;
	bIsPuzzleAlreadyStarted = false;
	OnPuzzleEnd.Broadcast();
}

void AHYPuzzleNPCBase::GiveReward()
{
	if (auto PC = Cast<AHYPlayerCharacController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		// Add rewards to player
		PC->Inventory.Add(RewardItem);
		
		// Reward over
		bIsFirstCleared = false;

		EndPuzzle();
		
		auto GameInst = Cast<UHYGameInstance>(GetGameInstance());
		FSaveData* SaveData = &GameInst->SaveData;
		
		// already in lore data
		if (auto Lore = SaveData->LoreData.Find(BossNPCRowName))
		{
			// If not received reward
			if (!Lore->bReceivedReward)
			{
				Lore->bReceivedReward = true;
				
				// Update lore date
				GameInst->UpdateLore(
					BossNPCRowName,
					*Lore
				);
			}
		}
		else
		{
			// Update lore(creation)
			GameInst->UpdateLore(
				BossNPCRowName,
				FLore(BossNPCRowName, true, false, true)
			);
		}
	}
}

void AHYPuzzleNPCBase::PuzzleSuccess()
{

}

void AHYPuzzleNPCBase::PuzzleFail()
{

}

void AHYPuzzleNPCBase::InitPuzzleCharacterStates()
{
	FSaveData* SaveData = &Cast<UHYGameInstance>(GetGameInstance())->SaveData;

	if (FLore* LoreData = SaveData->LoreData.Find(BossNPCRowName))
	{
		// If this character has been slain, then destroy
		if (LoreData->bHasBeenSlain)
		{
			Destroy();
		}
		// If not
		else
		{
			bIsFirstCleared = LoreData->bReceivedReward;
		}
	}
}
