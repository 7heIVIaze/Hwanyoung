// Copyright 2024. TEAM DAON. All rights reserved.


#include "HYSaveGameActor.h"
#include "Player/HYPlayerCharacController.h"

AHYSaveGameActor::AHYSaveGameActor()
{
	SavepointMesh = CreateDefaultSubobject<UStaticMeshComponent>("SavepointMesh");
	RootComponent = Cast<USceneComponent>(SavepointMesh);

	Super::Name = NSLOCTEXT("HYSaveGameActor", "InteractionName", "Haetae statue");
	Super::Action = NSLOCTEXT("HYSaveGameActor", "InteractionAction", "touch");
}

void AHYSaveGameActor::Interact_Implementation(APlayerController* Controller)
{
	Super::Interact_Implementation(Controller);

	AHYPlayerCharacController* IController = Cast<AHYPlayerCharacController>(Controller);
	IController->SaveGameplay();
}
