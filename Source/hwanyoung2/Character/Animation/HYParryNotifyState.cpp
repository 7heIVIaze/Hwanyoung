// Copyright 2024. TEAM DAON. All rights reserved.

#include "Character/Animation/HYParryNotifyState.h"
#include "Character/HYAggroNPCBase.h"

void UHYParryNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MeshComp)
	{
		if (auto AggroNPC = Cast<AHYAggroNPCBase>(MeshComp->GetOwner()))
		{
			AggroNPC->ToggleParryTiming(true);
		}
	}
}

void UHYParryNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp)
	{
		if (auto AggroNPC = Cast<AHYAggroNPCBase>(MeshComp->GetOwner()))
		{
			AggroNPC->ToggleParryTiming(false);
		}
	}
}