// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Dialogueable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDialogueable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HWANYOUNG2_API IDialogueable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void StartDialogue(class AHYPlayerCharacterBase* _PlayerRef) = 0;

	virtual void EndDialogue(class AHYPlayerCharacterBase* _PlayerRef) = 0;
};
