// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Buffable.h"
#include "Interfaces/WeaponInterface.h"
#include "HYWeaponBase.generated.h"

UCLASS()
class HWANYOUNG2_API AHYWeaponBase : public AActor, public IWeaponInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHYWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma region Functions

	UFUNCTION(BlueprintCallable)
	bool GenerateRandomDamageNumber(float _ComboFactor, float& _GeneratedDamage);

#pragma endregion

#pragma region Interfaces

	UFUNCTION(BlueprintCallable)
	virtual USkeletalMeshComponent* GetWeaponMesh() override { return Mesh; }

	UFUNCTION(BlueprintCallable)
	virtual bool ResetAttributes();

	// Buff character
	UFUNCTION(BlueprintCallable)
	virtual bool Buff(FAttributeSetInfo _AttributeSetInfo);

	// Debuff character
	UFUNCTION(BlueprintCallable)
	virtual bool Debuff(FAttributeSetInfo _AttributeSetInfo);

#pragma endregion

#pragma region Getter
	UFUNCTION(BlueprintCallable)
	UHYAttributeSystem* GetAttributeSystem() { return AttributeSystem; }
#pragma endregion

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Actor Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHYAttributeSystem> AttributeSystem;

	// Attribute set reference
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Actor Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHYAttributeSystem> AttributeSetRef;
};
