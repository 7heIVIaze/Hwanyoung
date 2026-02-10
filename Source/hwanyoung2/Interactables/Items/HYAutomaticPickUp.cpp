// Copyright 2024. TEAM DAON. All rights reserved.


#include "HYAutomaticPickUp.h"
#include "Player/HYPlayerCharacController.h"

// Sets default values
AHYAutomaticPickUp::AHYAutomaticPickUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	RootComponent = Cast<USceneComponent>(PickupMesh);

	ItemID = FName("No ID");

}

void AHYAutomaticPickUp::Collect_Implementation(APlayerController* Controller)
{
	AHYPlayerCharacController* IController = Cast<AHYPlayerCharacController>(Controller);
	if (IController->AddItemToInventoryByID(ItemID))
		Destroy();
}

FName AHYAutomaticPickUp::GetItemID()
{
	return ItemID;
}

// Called when the game starts or when spawned
void AHYAutomaticPickUp::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHYAutomaticPickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

