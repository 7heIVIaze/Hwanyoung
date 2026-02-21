// // Copyright 2024. TEAM DAON. All rights reserved.

#include "HYGroupManagerSubsystem.h"

#include "HYEnemyBase.h"

void UHYGroupManagerSubsystem::RegisterGroup(AHYEnemyBase* Mobs, FName GroupId)
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

void UHYGroupManagerSubsystem::RemoveFromGroup(AHYEnemyBase* Mobs, FName GroupId)
{
	Group[GroupId].Remove(Mobs);
}
