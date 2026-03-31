// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "HYAIController.generated.h"

/**
 * 
 */

#pragma region Teammates Did(Already Implemented)
UENUM(BlueprintType)
enum class EAISense
{
	None UMETA(DisplayName = "None"),
	Sight UMETA(DisplayName = "Sight"),
	Hearing UMETA(DisplayName = "Hearing"),
	Damage UMETA(DisplayName = "Damage"),
};
#pragma endregion

UCLASS()
class HWANYOUNG2_API AHYAIController : public AAIController
{
	GENERATED_BODY()

public: 
	AHYAIController();

	UFUNCTION(BlueprintCallable, Category = "AI|Group Battle")
	virtual void NoticeEnemy(AActor* InAttackTarget);

	UFUNCTION(BlueprintCallable, Category = "AI|Functions")
	void SetStateAsReturning();

public:
	// Blackboard key name
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	FName HomeLocationKeyName;

	// Initial spawned location
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	FVector OriginalLocation;

#pragma region Teammates Did(Already Implemented)

#pragma region EventGraphs
protected:
	virtual void OnPossess(APawn* PossessedPawn) override;

	UFUNCTION(BlueprintCallable, Category = "AI|Events")
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActor);

	UFUNCTION(BlueprintCallable, Category = "AI|Events")
	bool CanSenseActor(AActor* DetectedActor, EAISense Sense, FAIStimulus& Stimulus);
#pragma endregion

#pragma region Functions
public:
	UFUNCTION(BlueprintCallable, Category = "AI|Functions")
	void SetStateAsPassive();

	UFUNCTION(BlueprintCallable, Category = "AI|Functions")
	void SetStateAsAttacking(AActor* inAttackTarget, bool UseLastKnownAttackTarget);

	UFUNCTION(BlueprintCallable, Category = "AI|Functions")
	void SetStateAsInvestigating(FVector Location);

	UFUNCTION(BlueprintCallable, Category = "AI|Functions")
	void SetStateAsFrozen();

	UFUNCTION(BlueprintCallable, Category = "AI|Functions")
	void SetStateAsSeen(AActor* inAttackTarget, bool UseLastKnownAttackTarget);

	UFUNCTION(BlueprintCallable, Category = "AI|Functions")
	void HandleSensedSight(AActor* SensedActor);

	UFUNCTION(BlueprintCallable, Category = "AI|Functions")
	void HandleSensedSound(FVector Location);

	UFUNCTION(BlueprintCallable, Category = "AI|Functions")
	void HandleSensedDamage(AActor* SensedActor);

	UFUNCTION(BlueprintCallable, Category = "AI|Functions")
	enum ENPCBehaviorState GetCurrentState();

#pragma endregion

public:
	// Controlled Pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category ="AI")
	TObjectPtr<class AHYAggroNPCBase> CharacterRef;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "AI")
	AActor* AttackTarget;

	// Blackboard key name
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category ="AI")
	FName AttackRadiusKeyName;

	// Blackboard key name
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	FName DefendRadiusKeyName;

	// Blackboard key name
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	FName BehaviorStateKeyName;

	// Blackboard key name
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	FName PointOfInterestKeyName;

	// Blackboard key name
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	FName AttackTargetKeyName;

#pragma region Components

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	TObjectPtr<class UAIPerceptionComponent> AIPerception;
#pragma endregion

#pragma endregion

//protected:
//	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID);
//	virtual FGenericTeamId GetGenericTeamId() const;
};
