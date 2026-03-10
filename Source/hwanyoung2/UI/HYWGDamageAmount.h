// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataAssets/CombatTypes.h"
#include "HYWGDamageAmount.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API UHYWGDamageAmount : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

#pragma region Setter
	UFUNCTION(BlueprintCallable)
	void InitializeWidget(float InDamageAmount, EDamageType InDamageType);
#pragma endregion

public:
#pragma region Widget Hiarachy
	UPROPERTY(BlueprintReadWrite, Category = "Widget", meta = (BindWidget))
	TObjectPtr<class UTextBlock> DamageText;
#pragma endregion

public:
#pragma region Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Transient, Category = "Animation", meta = (BindWidgetAnim))
	class UWidgetAnimation* Fade;

	// damage amount for printing to damage text block
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widget")
	float DamageAmount;

	// determine the color of the damage text block
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widget")
	EDamageType DamageType;

	// for save the color
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget|UserEdit")
	TMap<EDamageType, FLinearColor> TypeColor;
#pragma endregion
};
