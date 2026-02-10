// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/CombatSystem.h"
#include "HYCombatSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTraceDamage, AActor*, _HitActor, const FHitResult&, _HitResult);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HWANYOUNG2_API UHYCombatSystem : public UActorComponent, public ICombatSystem
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHYCombatSystem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region Functions
	// Weapon trace for attack
	UFUNCTION(BlueprintCallable)
	bool PerformMultiTrace();
#pragma endregion

#pragma region Interfaces
	UFUNCTION(BlueprintCallable)
	virtual USkeletalMeshComponent* GetCharacterMesh() override { return Mesh; }

	UFUNCTION(BlueprintCallable)
	virtual void ToggleWeaponTrace(bool _WeaponTraceOn) override;

#pragma endregion

#pragma region Getter/Setter
	UFUNCTION(BlueprintCallable)
	bool GetIsAttacking() { return bIsAttacking; }
#pragma endregion

#pragma region Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	bool bIsAttacking;

	// For Attack combo timing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	bool bIsWithinResumeComboWindow;

	// For Attack combo timing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	bool bCanResumeCombo;

	// Location of Top socket
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variables")
	FVector PreviousTopSocket;

	// Location of bottom socket
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variables")
	FVector PreviousBottomSocket;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variables")
	TSet<AActor*> AlreadyDamaged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	float AttackDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variables")
	bool bWeaponTrace;

#pragma endregion

#pragma region Event Dispatchers
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnTraceDamage OnTraceDamage;
#pragma endregion
};
