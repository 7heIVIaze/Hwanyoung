// Copyright 2024. TEAM DAON. All rights reserved.

#include "UI/HYWGDamageAmount.h"

#include "Components/TextBlock.h"


void UHYWGDamageAmount::NativeConstruct()
{
	if (Fade)
	{
		PlayAnimation(Fade);
	}
	
	DamageText->SetText(FText::FromString(FString::SanitizeFloat(DamageAmount)));
	DamageText->SetColorAndOpacity(TypeColor[DamageType]);
}

void UHYWGDamageAmount::InitializeWidget(float InDamageAmount, EDamageType InDamageType)
{
	DamageAmount = InDamageAmount;
	DamageType = InDamageType;
}
