// Copyright 2024. TEAM DAON. All rights reserved.

#include "Character/Animation/HYPlayerAnimInstance.h"

#include "Character/HYPlayerCharacterBase.h"

void UHYPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	// Initialization code here
	PlayerCharacter = Cast<AHYPlayerCharacterBase>(TryGetPawnOwner());
}

void UHYPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	// Update code here
}

void UHYPlayerAnimInstance::AnimNotify_Death()
{
	// This function is called when the death animation notify is triggered.
	// You can add any logic here that needs to happen when the death animation plays.
	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->DeathNofity();
	}
}