// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYPlayerHUD.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Components/Image.h"

void UHYPlayerHUD::NativeConstruct()
{
	LowHPMI = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, M_LowHPOverlay);
	LowHPFX->SetBrushFromMaterial(LowHPMI);

	LowHPMI->SetScalarParameterValue(TEXT("Progress"), 100.0f);
}


