// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HYAttackComponent.generated.h"

// collision-based attack part
USTRUCT(BlueprintType)
struct FMeleeAttackPart {
	GENERATED_USTRUCT_BODY()

	//collider component (box collider)

	//attributes (floats):
	// base attack damage number
	// critical attack damage number
	// critical attack rate percentage

	//hitbox related info:
	// position of the hitbox relative to the player
	// 3D vector of the dimensions of the hitbox
	// 1 giant hit box (TODO: figure out a way to consider multiple parts as one hitbox entity to avoid looping)

	//method that returns boolean value based on whether there has been a collision detected with the enemy
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HWANYOUNG2_API UHYAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHYAttackComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//method that takes care of the dealing damage 
};
