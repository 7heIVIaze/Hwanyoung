// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HYAutomaticPickUp.generated.h"

UCLASS()
class HWANYOUNG2_API AHYAutomaticPickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHYAutomaticPickUp();

	UFUNCTION(BlueprintNativeEvent)
	void Collect(APlayerController* Controller);
	virtual void Collect_Implementation(APlayerController* Controller);

	FName GetItemID();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PickupMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
