// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HYBossField.generated.h"

UCLASS()
class HWANYOUNG2_API AHYBossField : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHYBossField();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma region Event Graphs
	// Begin overlap event for adding hp bar to viewport
	UFUNCTION(BlueprintCallable)
	void SphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndexBody, bool bFromSweep, const FHitResult& SweepResult);

	// End overlap event for remove hp bar from parent
	UFUNCTION(BlueprintCallable)
	void SphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndexBody);

	// Boss Fighting Finish
	UFUNCTION(BlueprintCallable)
	void Removeitself();
#pragma endregion

#pragma region Components
	// Using beginoverlap and endoverlap event of this component, adds Boss HP Bar widget to viewport.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USphereComponent> Sphere;
#pragma endregion

#pragma region Variables
	// Boss NPC Reference
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss|Variables")
	TObjectPtr<class AHYAggroNPCBase> BossNPCRef;

	// Boss NPC HP Bar Widget Reference
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss|Variables")
	TObjectPtr<class UHYBossHealthBar> BossBarWG;

	// Boss NPC HP Bar Widget Class
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss|Variables")
	TSubclassOf<class UUserWidget> BossBarWGClass;

	// Boss type data table for hp bar widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss|Variables")
	UDataTable* DT_BossType;
#pragma endregion
};
