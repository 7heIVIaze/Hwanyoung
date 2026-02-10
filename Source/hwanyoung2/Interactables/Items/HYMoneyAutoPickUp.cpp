// Copyright 2024. TEAM DAON. All rights reserved.


#include "HYMoneyAutoPickUp.h"
#include "Player/HYPlayerCharacController.h"

AHYMoneyAutoPickUp::AHYMoneyAutoPickUp()
{
	Super::ItemID = FName("Lost soul");
	Value = 1;
}

void AHYMoneyAutoPickUp::Collect_Implementation(APlayerController* Controller)
{
	AHYPlayerCharacController* IController = Cast<AHYPlayerCharacController>(Controller);
	IController->Currency += Value;
	Destroy();
}
