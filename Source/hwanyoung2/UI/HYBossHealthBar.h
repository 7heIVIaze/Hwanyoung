// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HYBossHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API UHYBossHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
#pragma region Components
protected:
	// Back HP bar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss", meta = (BindWidget))
	TObjectPtr<class UProgressBar> Back;

	// Front HP bar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss", meta = (BindWidget))
	TObjectPtr<class UProgressBar> Front;
	
	// Boss name text block
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss", meta = (BindWidget))
	TObjectPtr<class UTextBlock> BossName;

	// Boss title text block
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss", meta = (BindWidget))
	TObjectPtr<class UTextBlock> BossTitle;
#pragma endregion

#pragma region Event Graphs
public:
	UFUNCTION(BlueprintCallable)
	void SetPercentage();

	UFUNCTION(BlueprintCallable)
	void SetBossName(struct FBossType BossType);
#pragma endregion

#pragma region Variables
public:
	// Boss NPC reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	AActor* DamagableActor;

	// Front HP bar percentage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float TargetPercentage;

	// Back HP bar percentage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float BackPercentage;

	// For decreasing linearly of back HP bar.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variables")
	UCurveFloat* BackBarCurve;
#pragma endregion

private:
	float ElapsedTime = 0.0f;
	bool bIsHPAnimating = false;
};
