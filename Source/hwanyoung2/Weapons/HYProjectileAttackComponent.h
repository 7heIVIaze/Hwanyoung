// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HYProjectileAttackComponent.generated.h"

// hitscan-based attack part
USTRUCT(BlueprintType)
struct FProjectileAttackPart {
	GENERATED_USTRUCT_BODY()

	//line trace component

	//attributes (floats):
	// base attack damage number
	// critical attack damage number
	// critical attack rate percentage

	//line trace related info:
	// where is the source of line trace placed? 
	// origin of the raycast

	//method that returns boolean value based on whether the line trace has hit the enemy 
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HWANYOUNG2_API UHYProjectileAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHYProjectileAttackComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	//method that takes care of the dealing damage 

		
};
