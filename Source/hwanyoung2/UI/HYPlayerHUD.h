// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HYPlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API UHYPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

#pragma region Teammates Did(Already Implemented)
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials", meta = (BindWidget))
	TObjectPtr<class UImage> LowHPFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials", meta = (BindWidget))
	TObjectPtr<class UProgressBar> ESkillBar;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Materials")
	class UMaterialInstanceDynamic* LowHPMI;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Materials")
	class UMaterialInterface* M_LowHPOverlay;
#pragma endregion
};
