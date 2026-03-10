// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HYHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API UHYHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
#pragma region Functions
	UFUNCTION(BlueprintCallable, Category = "HPChanged")
	virtual void SetDamagableActor(AActor* inDamagableActor);

	UFUNCTION(BlueprintCallable, Category = "HPChanged")
	virtual void SetPercentage();

	// Timeline hp
	UFUNCTION(BlueprintCallable, Category = "HPChanged")
	void UpdateBackBar();
#pragma endregion

#pragma region Variables
	// The reference of actor who will have this widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variables")
	TObjectPtr<class AActor> DamagableActor;

	// The percentage of front HP bar
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variables")
	float FrontPercentage;

	// The percentage of back HP bar
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variables")
	float BackPercentageHolder;

	// For decreasing linearly of back HP bar .
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variables")
	UCurveFloat* BackBarCurve;

	UPROPERTY(BlueprintReadWrite, Category = "HP Bar")
	FTimerHandle BackBarUpdateTimer;
#pragma endregion

#pragma region Widget Hiarachy
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widgets", meta=(BindWidget))
	TObjectPtr<class UProgressBar> BackBar;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widgets", meta=(BindWidget))
	TObjectPtr<class UProgressBar> FrontBar;
#pragma endregion

private:
	float ElapsedTime = 0.0f;
	bool bIsHPAnimating = false;
};
