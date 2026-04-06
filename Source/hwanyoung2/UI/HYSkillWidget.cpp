// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYSkillWidget.h"

#include "Components/ProgressBar.h"

void UHYSkillWidget::SkillCooldownPercent(bool WasCastByPlayer, float LeftCooldownTime)
{
	if(WasCastByPlayer)
	{
		// Show this skill is locked, until the skill cooldown is over.
		CooldownBar->SetPercent(1.0f);
	}
	else
	{
		// Show this skill is locked during left cooldown time.
		CooldownBar->SetPercent(LeftCooldownTime);
	}
}
