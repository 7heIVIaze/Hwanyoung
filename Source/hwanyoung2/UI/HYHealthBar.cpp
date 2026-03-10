// Copyright 2024. TEAM DAON. All rights reserved.

#include "UI/HYHealthBar.h"
#include "Components/ProgressBar.h"
#include "Interfaces/Damagable.h"

void UHYHealthBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsHPAnimating && BackBarCurve)
	{
		ElapsedTime += InDeltaTime;

		float MinTime, MaxTime;
		BackBarCurve->GetTimeRange(MinTime, MaxTime);

		float CurveValue = BackBarCurve->GetFloatValue(ElapsedTime);

		float BackBarPercent = FMath::Lerp(BackPercentageHolder, FrontPercentage, CurveValue);
		BackBar->SetPercent(BackBarPercent);

		if (ElapsedTime >= MaxTime)
		{
			bIsHPAnimating = false;
		}
	}
}

void UHYHealthBar::SetDamagableActor(AActor* inDamagableActor)
{
	DamagableActor = inDamagableActor;
}

void UHYHealthBar::SetPercentage()
{
	if (auto DamageInterface = Cast<IDamagable>(DamagableActor))
	{
		float MaxHP = DamageInterface->GetMaxHP();
		float CurrentHP = DamageInterface->GetCurrentHP();

		FrontPercentage = CurrentHP / MaxHP;
	}

	FrontBar->SetPercent(FrontPercentage);

	BackPercentageHolder = BackBar->GetPercent();

	ElapsedTime = 0.0f;
	bIsHPAnimating = true;

	//// Start the timer for reduce the time linearly.
	//GetWorld()->GetTimerManager().SetTimer(BackBarUpdateTimer, this, 
	//	&UHYHealthBar::UpdateBackBar, 0.01f, true);
}

void UHYHealthBar::UpdateBackBar()
{
	float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(BackBarUpdateTimer);

	float CurveAlpha = BackBarCurve->GetFloatValue(ElapsedTime);

	float BackBarPercent = FMath::Lerp(BackPercentageHolder, FrontPercentage, CurveAlpha);
	BackBar->SetPercent(BackBarPercent);

	
	if (ElapsedTime >= 0.3f)
	{
		GetWorld()->GetTimerManager().ClearTimer(BackBarUpdateTimer);
	}
}


