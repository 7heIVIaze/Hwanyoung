// Copyright 2024. TEAM DAON. All rights reserved.

#include "UI/HYBossHealthBar.h"

#include "DataAssets/HwanyoungType.h"
#include "Interfaces/Damagable.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHYBossHealthBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsHPAnimating && BackBarCurve)
	{
		ElapsedTime += InDeltaTime;

		float MinTime, MaxTime;
		BackBarCurve->GetTimeRange(MinTime, MaxTime);

		float CurveValue = BackBarCurve->GetFloatValue(ElapsedTime);

		float BackBarPercent = FMath::Lerp(BackPercentage, TargetPercentage, CurveValue);
		Back->SetPercent(BackBarPercent);

		if (ElapsedTime >= MaxTime)
		{
			bIsHPAnimating = false;
		}
	}
}

void UHYBossHealthBar::SetPercentage()
{
	if (auto DamagableInterface = Cast<IDamagable>(DamagableActor))
	{
		TargetPercentage =
			DamagableInterface->GetCurrentHP() / DamagableInterface->GetMaxHP();
	}

	Front->SetPercent(TargetPercentage);

	BackPercentage = Back->Percent;

	ElapsedTime = 0.0f;
	bIsHPAnimating = true;
}

void UHYBossHealthBar::SetBossName(FBossType BossType)
{
	BossName->SetText(BossType.BossName);
	BossName->SetColorAndOpacity(FSlateColor(BossType.BossNameColor));

	BossTitle->SetText(BossType.BossTitle);
}