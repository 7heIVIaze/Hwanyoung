// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYAIController.h"

#include "Character/HYAggroNPCBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Damage.h"
#include "DataAssets/CharacterStatusType.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

AHYAIController::AHYAIController()
{
	//if we create a new AI instance, it will want to have its own player state:
	bWantsPlayerState = true;

#pragma region Teammates Did(Already Implemented)
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
#pragma endregion
}

void AHYAIController::NoticeEnemy(AActor* InAttackTarget)
{
	// Set attack target and change the current state as attacking
	HandleSensedDamage(InAttackTarget);
}

void AHYAIController::SetStateAsReturning()
{
	Blackboard->SetValueAsEnum(BehaviorStateKeyName, (uint8)ENPCBehaviorState::Returnning);
	CharacterRef->Widget->SetVisibility(false);
}

#pragma region Teammates Did(Already Implemented)
void AHYAIController::OnPossess(APawn* PossessedPawn)
{
	CharacterRef = Cast<AHYAggroNPCBase>(PossessedPawn);
	if (CharacterRef == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No Valid Behavior Tree Available"));
	}

	RunBehaviorTree(CharacterRef->BehaviorTree);
	SetStateAsPassive();

	float AttackRadius, DefendRadius;
	CharacterRef->GetIdealRange(AttackRadius, DefendRadius);

	Blackboard->SetValueAsFloat(AttackRadiusKeyName, AttackRadius);
	Blackboard->SetValueAsFloat(DefendRadiusKeyName, DefendRadius);

	// Save the character's home location for returning to its original location.
	Blackboard->SetValueAsVector(HomeLocationKeyName, CharacterRef->GetActorLocation());

}
void AHYAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActor)
{
	for (auto DetectedActor : UpdatedActor)
	{
		FAIStimulus Stimulus;
		CanSenseActor(DetectedActor, EAISense::Sight, Stimulus);
		HandleSensedSight(DetectedActor);

		bool bHasSensed = CanSenseActor(DetectedActor, EAISense::Hearing, Stimulus);
		if (bHasSensed)
		{
			HandleSensedSound(Stimulus.StimulusLocation);
		}

		bHasSensed = CanSenseActor(DetectedActor, EAISense::Damage, Stimulus);
		if (bHasSensed)
		{
			HandleSensedDamage(DetectedActor);
		}
	}
}

bool AHYAIController::CanSenseActor(AActor* DetectedActor, EAISense Sense, FAIStimulus& Stimulus)
{
	FActorPerceptionBlueprintInfo PerceptionInfo;
	AIPerception->GetActorsPerception(DetectedActor, PerceptionInfo);

	for (const FAIStimulus& Stimuli : PerceptionInfo.LastSensedStimuli)
	{
		TSubclassOf<UAISense> SensedClass = UAIPerceptionSystem::GetSenseClassForStimulus(this, Stimulus);
		TSubclassOf<UAISense> CompareSensed;
		switch (Sense)
		{
			case EAISense::Sight:
			{
				CompareSensed = UAISense_Sight::StaticClass();
			}
			case EAISense::Hearing:
			{
				CompareSensed = UAISense_Hearing::StaticClass();
			}
			case EAISense::Damage:
			{
				CompareSensed = UAISense_Damage::StaticClass();
			}
		}

		if (SensedClass == CompareSensed)
		{
			Stimulus = Stimuli;
			return Stimuli.WasSuccessfullySensed();
		}
		else
		{
			Stimulus = Stimuli;
			return false;
		}

	}

	return false;
}

void AHYAIController::SetStateAsPassive()
{
	Blackboard->SetValueAsEnum(BehaviorStateKeyName, (uint8)ENPCBehaviorState::Passive);
	CharacterRef->Widget->SetVisibility(false);
}

void AHYAIController::SetStateAsAttacking(AActor* inAttackTarget, bool UseLastKnownAttackTarget)
{
	if (CharacterRef->IsDead())
	{
		AttackTarget = nullptr;
		return;
	}

	AActor* NewAttackTarget =
		(IsValid(AttackTarget) && UseLastKnownAttackTarget) ? AttackTarget : inAttackTarget;

	if (!IsValid(NewAttackTarget))
	{
		SetStateAsPassive();
	}

	Blackboard->SetValueAsObject(AttackTargetKeyName, NewAttackTarget);

	Blackboard->SetValueAsEnum(BehaviorStateKeyName, (uint8)ENPCBehaviorState::Combat);
	AttackTarget = NewAttackTarget;
	CharacterRef->Widget->SetVisibility(true);

}

void AHYAIController::SetStateAsInvestigating(FVector Location)
{
	Blackboard->SetValueAsEnum(BehaviorStateKeyName, (uint8)ENPCBehaviorState::Investigating);
	Blackboard->SetValueAsVector(PointOfInterestKeyName, Location);
}

void AHYAIController::SetStateAsFrozen()
{
	Blackboard->SetValueAsEnum(BehaviorStateKeyName, (uint8)ENPCBehaviorState::Frozen);
}

void AHYAIController::SetStateAsSeen(AActor* inAttackTarget, bool UseLastKnownAttackTarget)
{
	AActor* NewAttackTarget =
		(UseLastKnownAttackTarget && IsValid(AttackTarget)) ? AttackTarget : inAttackTarget;
	
	if (!IsValid(NewAttackTarget))
	{
		SetStateAsPassive();
	}

	Blackboard->SetValueAsObject(AttackTargetKeyName, NewAttackTarget);

	Blackboard->SetValueAsEnum(BehaviorStateKeyName, (uint8)ENPCBehaviorState::Seen);
	AttackTarget = NewAttackTarget;
	CharacterRef->Widget->SetVisibility(true);

}

void AHYAIController::HandleSensedSight(AActor* SensedActor)
{
	ENPCBehaviorState CurrentState = GetCurrentState();

	switch (CurrentState)
	{
		case ENPCBehaviorState::Passive:
		case ENPCBehaviorState::Investigating:
			if (SensedActor == UGameplayStatics::GetPlayerCharacter(this, 0))
			{
				SetStateAsAttacking(SensedActor, false);
			}
			break;
	}
}

void AHYAIController::HandleSensedSound(FVector Location)
{
	ENPCBehaviorState CurrentState = GetCurrentState();

	switch (CurrentState)
	{
	case ENPCBehaviorState::Passive:
	case ENPCBehaviorState::Investigating:
			SetStateAsInvestigating(Location);
		break;
	}
}

void AHYAIController::HandleSensedDamage(AActor* SensedActor)
{
	ENPCBehaviorState CurrentState = GetCurrentState();

	switch (CurrentState)
	{
	case ENPCBehaviorState::Passive:
	case ENPCBehaviorState::Investigating:
		SetStateAsAttacking(SensedActor, true);
		break;
	}
}
	
ENPCBehaviorState AHYAIController::GetCurrentState()
{
	return (ENPCBehaviorState)Blackboard->GetValueAsEnum(BehaviorStateKeyName);
}
#pragma endregion