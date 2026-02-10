// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HYInteractableActor.generated.h"

/*
* Parent class for all interactable actor objects 
*/
UCLASS()
class HWANYOUNG2_API AHYInteractableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHYInteractableActor();

	//this is a Blueprint-only Event; does not have C++ implementation
	UFUNCTION(BlueprintNativeEvent)
	void Interact(APlayerController* Controller);

	virtual void Interact_Implementation(APlayerController* Controller);

	UPROPERTY(EditDefaultsOnly)
	FText Name;

	UPROPERTY(EditDefaultsOnly)
	FText Action;

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	virtual FText GetInteractText() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
