// Copyright 2024. TEAM DAON. All rights reserved.


#include "HYInteractableActor.h"

// Sets default values
AHYInteractableActor::AHYInteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AHYInteractableActor::Interact_Implementation(APlayerController* Controller)
{
	return;
}

FText AHYInteractableActor::GetInteractText() const
{
	
	FText ReturnText = FText::Format(NSLOCTEXT("HYInteractableActor", "HowToInteraction", "{0}: Press F to {1}"), Name, Action);
	return ReturnText;
	//return FString::Printf(TEXT("%s: Press F to %s"), *Name, *Action);
}

// Called when the game starts or when spawned
void AHYInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHYInteractableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

