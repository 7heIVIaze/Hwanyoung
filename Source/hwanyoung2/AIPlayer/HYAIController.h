// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "HYAIController.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API AHYAIController : public AAIController
{
	GENERATED_BODY()

protected:


	
public: 
	AHYAIController();

	UFUNCTION(BlueprintCallable, Category = "AI|Group Battle")
	virtual void NoticeEnemy(AActor* InAttackTarget);

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "AI")
	AActor* AttackTarget;

//protected:
//	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID);
//	virtual FGenericTeamId GetGenericTeamId() const;
};
