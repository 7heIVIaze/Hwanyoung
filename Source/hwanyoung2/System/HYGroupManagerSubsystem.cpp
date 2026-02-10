// // Copyright 2024. TEAM DAON. All rights reserved.

#include "HYGroupManagerSubsystem.h"

#include "Character/HYAggroNPCBase.h"

void UHYGroupManagerSubsystem::RegisterGroup(AHYAggroNPCBase* Mobs, FName GroupId)
{
	Group.FindOrAdd(GroupId).Add(Mobs);
}

void UHYGroupManagerSubsystem::NotifyGroupAttacked(FName GroupId, AActor* AttackTarget)
{
	for (auto Mob : Group[GroupId])
	{
		Mob->NotifyGroupAttacked(AttackTarget);
	}
}