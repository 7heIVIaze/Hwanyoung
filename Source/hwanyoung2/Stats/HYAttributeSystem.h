// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HYAttributeSystem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HWANYOUNG2_API UHYAttributeSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHYAttributeSystem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
#pragma region Functions
	// Buff current attributes
	UFUNCTION(BlueprintCallable)
	bool Buff(FStatBuffInfo _AttributeInfo);

	// Reset all attributes
	UFUNCTION(BlueprintCallable)
	bool ResetAttributes();

	// Debuff current attributes
	UFUNCTION(BlueprintCallable)
	bool Debuff(FStatBuffInfo _AttributeInfo);

	// Remove buff current attributes
	UFUNCTION(BlueprintCallable)
	bool RemoveBuff(FStatBuffInfo _AttributeInfo);

	// Remove debuff current attributes
	UFUNCTION(BlueprintCallable)
	bool RemoveDebuff(FStatBuffInfo _AttributeInfo);

	// Calculate all stats in the final
	UFUNCTION(BlueprintCallable)
	void CalculateFinalStats();
#pragma endregion

#pragma region Setter/Getter
	// Getter
	UFUNCTION(BlueprintCallable)
	float GetCurrentPhysicalDamage() { return CurrentPhysDMG; }

	UFUNCTION(BlueprintCallable)
	float GetCurrentCriticalDamage() { return CurrentCritDMG; }

	UFUNCTION(BlueprintCallable)
	float GetCurrentCriticalRate() { return CurrentCritRate; }

	UFUNCTION(BlueprintCallable)
	float GetCurrentDefense() { return CurrentDEF; }
#pragma endregion

#pragma region Variables
private:
	// Current physical damage amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeSet", meta = (AllowPrivateAccess = "true"))
	float CurrentPhysDMG;

	// Current critical damage amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeSet", meta = (AllowPrivateAccess = "true"))
	float CurrentCritDMG;

	// Current critical rate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeSet", meta = (AllowPrivateAccess = "true"))
	float CurrentCritRate;

	// Current defense
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeSet", meta = (AllowPrivateAccess = "true"))
	float CurrentDEF;

	// Original physical damage amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeSet", meta = (AllowPrivateAccess = "true"))
	float OriginalPhysDMG;

	// Original critical damage amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeSet", meta = (AllowPrivateAccess = "true"))
	float OriginalCritDMG;

	// Original critical rate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeSet", meta = (AllowPrivateAccess = "true"))
	float OriginalCritRate;

	// Original defense
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeSet", meta = (AllowPrivateAccess = "true"))
	float OriginalDEF;

	// Multiplied buff factor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeSet", meta = (AllowPrivateAccess = "true"))
	FAttributeSetInfo MultipliedBuffFactor;

	// Multiplied debuff factor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeSet", meta = (AllowPrivateAccess = "true"))
	FAttributeSetInfo MultipliedDebuffFactor;

	// Added buff factor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeSet", meta = (AllowPrivateAccess = "true"))
	FAttributeSetInfo AddedBuffFactor;

	// Added debuff factor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeSet", meta = (AllowPrivateAccess = "true"))
	FAttributeSetInfo AddedDebuffFactor;
#pragma endregion
};
