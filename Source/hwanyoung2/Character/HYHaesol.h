// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HYPlayerCharacterBase.h"
#include "HYHaesol.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API AHYHaesol : public AHYPlayerCharacterBase
{
	GENERATED_BODY()

public:
	AHYHaesol();
	
#pragma region Event Graphs
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Haesol's third ability
	virtual void UseThirdAbility_Implementation() override;

	virtual void CSkillCooldown_Implementation() override;

	virtual void UseUltimateAbility_Implementation() override;

	virtual void UltSkillCooldown_Implementation() override;

	virtual void ResetQSkill() override;

	virtual void ResetCSkill() override;
	
	virtual void ResetESkill() override;
	
	virtual void ResetUltSkill() override;

	// After Casting, Actually used skill effect
	UFUNCTION(BlueprintNativeEvent)
	void ActivateThirdSkill();

	virtual void ActivateThirdSkill_Implementation();

	// After Casting, Actually used skill effect
	UFUNCTION(BlueprintNativeEvent)
	void ActivateUltSkill();

	virtual void ActivateUltSkill_Implementation();

	// Blocked cpp based implementation.
	virtual void Blocked_Implementation(bool _CanBeParried, AActor* _DamageInstigator) override;

	// Parry Success cpp based implementation
	virtual void ParrySuccess_Implementation(AActor* _DamageInstigator) override;

	// Parry Failed cpp based implementation
	virtual void ParryFailed_Implementation(AActor* _DamageInstigator) override;

#pragma endregion

#pragma region Functions

#pragma endregion

#pragma region Variables
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterAbility)
	UMaterialInterface* M_HaesolProjectilePathAOE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterAbility)
	UMaterialInterface* M_HaesolUltPathAOE;

	// C Skill AOE Actor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterAbility)
	TSubclassOf<AActor> HeavenlyArrowClass;

	// Ult Skill AOE Actor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterAbility)
	TSubclassOf<AActor> HaesolUltClass;
#pragma endregion

#pragma region Switcher
	bool bIsShowProjPathOpened = false;
#pragma endregion
};
