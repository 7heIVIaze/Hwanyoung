// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HYPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API UHYPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;

	void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UFUNCTION(BlueprintCallable, Category = "Notify|Death")
	void AnimNotify_Death();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	class AHYPlayerCharacterBase* PlayerCharacter;
};
