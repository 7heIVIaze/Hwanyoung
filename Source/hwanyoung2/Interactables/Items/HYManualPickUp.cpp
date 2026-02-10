// Copyright 2024. TEAM DAON. All rights reserved.


#include "HYManualPickUp.h"
#include "Player/HYPlayerCharacController.h"

AHYManualPickUp::AHYManualPickUp()
{
	PickUpRangeMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	RootComponent = Cast<USceneComponent>(PickUpRangeMesh);

	ItemID = FName("No ID");

	Super::Name = NSLOCTEXT("HYManualPickUp", "InteractionName", "Item");
	Super::Action = NSLOCTEXT("HYManualPickUp", "InteractionAction", "pick up");
}

void AHYManualPickUp::Interact_Implementation(APlayerController* Controller)
{
	Super::Interact_Implementation(Controller);

	AHYPlayerCharacController* IController = Cast<AHYPlayerCharacController>(Controller);
	if (IController->AddItemToInventoryByID(ItemID))
		Destroy();
	else UE_LOG(LogTemp, Warning, TEXT("Item not found."))
}

