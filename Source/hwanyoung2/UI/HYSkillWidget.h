// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HYSkillWidget.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API UHYSkillWidget : public UUserWidget
{
	GENERATED_BODY()

// Ignore the components which is skill icon, circular progress bar(UImage).
#pragma region Widget Components
public:
	// Skill Cooldown bar
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UProgressBar> CooldownBar;
#pragma endregion

#pragma region Event Graphs
public:
	/// <summary>
	/// This is for showing this skill is locked. But the circular progress bar will do the actual cooldown VFX.
	/// </summary>
	/// <param name="WasCastByPlayer">Was this skill cast by player?</param>
	/// <param name="LeftCooldownTime">How much time left to unlock this skill.</param>
	UFUNCTION(BlueprintCallable)
	void SkillCooldownPercent(bool WasCastByPlayer, float LeftCooldownTime);
	
#pragma endregion
};
