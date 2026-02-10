// Copyright 2024. TEAM DAON. All rights reserved.
// This class is the parent class of all boss NPCs.
#pragma once

#include "CoreMinimal.h"
#include "HYAggroNPCBase.h"
#include "DataAssets/DialogueType.h"
#include "Inventory/InventoryItem.h"
#include "Components/TimelineComponent.h"
#include "HYBossNPCBase.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API AHYBossNPCBase : public AHYAggroNPCBase
{
	GENERATED_BODY()

public:
	AHYBossNPCBase();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

#pragma region Event Graphs
	// This function will be called after animation finished(Use Super LMAO)
	virtual void Death_Implementation() override;
	
	// Dialogue End event (bound with dialogue component)
	UFUNCTION(BlueprintCallable)
	virtual void DialogueEndEvent(EDialogueEventType _EventTriggerType);

	// Any animation montage after called
	UFUNCTION(BlueprintCallable)
	virtual void OnMontageEnded(UAnimMontage* _Montage, bool _Interrupted);

	UFUNCTION(BlueprintCallable)
	virtual void DiedMontageEnded();

	UFUNCTION(BlueprintCallable)
	void DissolveMesh(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void DissolveFinished();
#pragma endregion

#pragma region Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference|Souls")
	TSubclassOf<AActor> SoulClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference|Souls")
	FInventoryItem RewardItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DeathAnimMontage;

	FTimeline DeathMeshTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UCurveFloat* DeathTimelineCurveFloat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FName CombatFinalDialogueLine;
#pragma endregion
	
};
