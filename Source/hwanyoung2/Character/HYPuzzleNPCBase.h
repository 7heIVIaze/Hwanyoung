// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HYEnemyBase.h"
#include "Interfaces/CharacterState.h"
#include "Interfaces/NPCBehavior.h"
#include "Interfaces/Dialogueable.h"
#include "Interfaces/Damagable.h"
#include "HYPuzzleNPCBase.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHPChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPuzzleStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPuzzleEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPuzzleSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPuzzleFailed);

UCLASS()
class HWANYOUNG2_API AHYPuzzleNPCBase : public AHYEnemyBase, public ICharacterState, public INPCBehavior, public IDialogueable, public IDamagable
{
	GENERATED_BODY()
	

public:
	AHYPuzzleNPCBase();

	virtual void BeginPlay() override;

#pragma region Event Graphs
	UFUNCTION(BlueprintNativeEvent)
	void Death();

	virtual void Death_Implementation();

	// Dialogue End event function : this function will be called when dialogue finish and the event occurs.
	UFUNCTION(BlueprintCallable)
	virtual void DialogueEndEvent(EDialogueEventType EventTriggerType);

	// Start puzzle contents : this function will be implemented in the child class
	UFUNCTION(BlueprintCallable)
	virtual void StartPuzzleContent();

	// Puzzle Finished
	UFUNCTION(BlueprintCallable)
	virtual void EndPuzzle();

	// Give rewards to player after succeed puzzle
	UFUNCTION(BlueprintCallable)
	virtual void GiveReward();

	// Puzzle success function(Dialogue Start)
	UFUNCTION(BlueprintCallable)
	virtual void PuzzleSuccess();

	// Puzzle fail function(Dialogue Start)
	UFUNCTION(BlueprintCallable)
	virtual void PuzzleFail();

	// Initialize puzzle npc's state from save data(puzzle success, npc died, etc.)
	UFUNCTION(BlueprintCallable)
	virtual void InitPuzzleCharacterStates();

#pragma endregion

#pragma region Components
	// Actor component for managing dialogue
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UHYDialogueComponent> DialogueSystem;

	// Manage location to show hit VFX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USceneComponent> HitVFXPoint;

	// Widget component for HP Bar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UWidgetComponent> Widget;

	// HP System
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UHYHPSystem> HPSystem;

#pragma endregion

#pragma region Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<AActor> PatrolRoute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<class UBehaviorTree> BehaviorTree;

	// AI Controller
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<class AHYAIController> AICBase;

	// Puzzle Cleared State
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Puzzle|State")
	bool bIsFirstCleared;

	// Can Puzzle Start State: true(puzzle start), false(run away from player or puzzle started)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Puzzle|State")
	bool bCanStartPuzzle;

	// for preventing duplicate execution of puzzle
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Puzzle|State")
	bool bIsPuzzleAlreadyStarted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State", meta = (Bitmask, BitmaskEnum = "ECharacterState"))
	uint8 CharacterState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float DamageAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Name")
	FName BossNPCRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	ENPCMovementSpeed CurrentMovementType;

	// Animation for stagger
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimMontage> StaggerMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation|Death")
	TArray<UMaterialInstanceDynamic*> DissolveMeshMaterial;

	// Blood Hit VFX(Decal)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood")
	TObjectPtr<UMaterialInterface> M_DarkEnergyDecal;

	// Blood Hit VFX(Particles)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood")
	TObjectPtr<UNiagaraSystem> NS_DarkEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood")
	FVector BloodDecalSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blood")
	FVector HitParticleScale;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	FTimerHandle DOTTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	FTimerHandle EndTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	FTimerHandle HitStopTimer;
#pragma endregion

#pragma region resource
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	TSubclassOf<class UUserWidget> NPCHealthWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	TArray<TObjectPtr<class UNiagaraSystem>> HitEffects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	TArray<TObjectPtr<class UFMODEvent>> HitEvents;

	// Puzzle rewards
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	FInventoryItem RewardItem;

	// Damage Amount class for spawning.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	TSubclassOf<class UUserWidget> DamageAmountClass;
	//TSubclassOf<class ADamageAmount> DamageAmountClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	TObjectPtr<class UAnimMontage> PlayerStaggerMontage;
#pragma endregion

public:
#pragma region Event Dispatcher
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnHPChanged OnHPChanged;

	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnDeath OnDeath;

	// Dialogue End event
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnDialogueEnd OnDialogueEnd;

	// Puzzle Start event
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnPuzzleStart OnPuzzleStart;

	// Puzzle finish event
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnPuzzleStart OnPuzzleEnd;

	// Puzzle success event
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnPuzzleStart OnPuzzleSuccess;

	// Puzzle fail event
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnPuzzleStart OnPuzzleFail;
#pragma endregion
};
