// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYBossNPCBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/HYPlayerCharacController.h"
#include "HYGameInstance.h"
#include "DialogueableSystem/HYDialogueComponent.h"
#include "HYPlayerCharacterBase.h"

AHYBossNPCBase::AHYBossNPCBase()
{

}

void AHYBossNPCBase::BeginPlay()
{
	Super::BeginPlay();

	if (DialogueSystem)
	{
		DialogueSystem->OnDialogueEventTrigger.AddDynamic(this, &AHYBossNPCBase::DialogueEndEvent);
	}

	if (DeathTimelineCurveFloat)
	{
		// Timeline Update
		FOnTimelineFloat DissolveMeshFunction;
		DissolveMeshFunction.BindUFunction(this, FName("DissolveMesh"));
		DeathMeshTimeline.AddInterpFloat(DeathTimelineCurveFloat, DissolveMeshFunction);

		// Timeline Finished
		FOnTimelineEvent DissolveMeshFinished;
		DissolveMeshFinished.BindUFunction(this, FName("DissolveFinished"));
		DeathMeshTimeline.SetTimelineFinishedFunc(DissolveMeshFinished);
	}
}

void AHYBossNPCBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DeathMeshTimeline.TickTimeline(DeltaTime);
}

void AHYBossNPCBase::Death_Implementation()
{
	Super::Death_Implementation();

	DialogueSystem->RowName = CombatFinalDialogueLine;
	StartDialogue(Cast<AHYPlayerCharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0)));
}

void AHYBossNPCBase::DialogueEndEvent(EDialogueEventType _EventTriggerType)
{
	switch (_EventTriggerType)
	{
		// Selected Boss NPC Live
		case EDialogueEventType::None:
		{
			// Spawn 2 souls(money).
			GetWorld()->SpawnActor<AActor>(SoulClass, GetActorTransform());
			GetWorld()->SpawnActor<AActor>(SoulClass, GetActorTransform());

			// And Give reward items
			if (AHYPlayerCharacController* PlayerController =
				Cast<AHYPlayerCharacController>(UGameplayStatics::GetPlayerController(this, 0)))
			{
				PlayerController->Inventory.Add(RewardItem);
			}
			break;
		}
		// Selected Kill Boss NPC
		case EDialogueEventType::Death:
		{
			GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &AHYBossNPCBase::OnMontageEnded);
			//GetMesh()->PlayAnimation(DeathAnimMontage, false);
			PlayAnimMontage(DeathAnimMontage);
			
			
			break;
		}
		// Battle Start
		case EDialogueEventType::BattleStart:
		{
			OnDialogueEnd.Broadcast();
		}
	}
}

void AHYBossNPCBase::OnMontageEnded(UAnimMontage* _Montage, bool _Interrupted)
{
	// Current Montage is Death Montage
	if (_Montage == DeathAnimMontage)
	{
		DiedMontageEnded();
	}
}

void AHYBossNPCBase::DiedMontageEnded()
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Create death material instance for dissolve mesh
	CreateDeathMaterialInstance();

	DeathMeshTimeline.PlayFromStart();
}

void AHYBossNPCBase::DissolveMesh(float DeltaTime)
{
	for (auto DissolveMat : DissolveMeshMaterial)
	{
		DissolveMat->SetScalarParameterValue(FName(TEXT("DissolveFactor")), FMath::Lerp(-1.0f, 1.0f, DeltaTime));
	}
}

void AHYBossNPCBase::DissolveFinished()
{
	// Spawn 1 soul
	GetWorld()->SpawnActor<AActor>(SoulClass, GetActorTransform());
	DissolveMeshMaterial.Empty();

	// Update Lore data
	UHYGameInstance* GameInstance = Cast<UHYGameInstance>(GetGameInstance());

	if (GameInstance)
	{
		// If the lore of this boss is already saved
		if (GameInstance->SaveData.LoreData.Find(BossNPCRowName))
		{
			// Only Update hasbeenslain info.
			GameInstance->SaveData.LoreData[BossNPCRowName].bHasBeenSlain = true;
		}
		// If it is the first, overwrite new lore data.
		else
		{
			FLore NewLore(BossNPCRowName, true, true, true);
			GameInstance->SaveData.LoreData[BossNPCRowName] = NewLore;
		}
	}

	Destroy();
}