// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "HYParryNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API UHYParryNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
protected:
	// Enable to be parried
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;

	// Disable to be parried
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
