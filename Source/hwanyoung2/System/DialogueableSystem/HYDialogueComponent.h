// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/Dialogueable.h"
#include "HYDialogueComponent.generated.h"

class FMODEvent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueEventTrigger, EDialogueEventType, _EventTriggerType);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HWANYOUNG2_API UHYDialogueComponent : public UActorComponent, public IDialogueable
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHYDialogueComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region EventGraphs
	UFUNCTION(BlueprintNativeEvent)
	void OptionSelected(UDataTable* _DataTable, FName _RowName);

	virtual void OptionSelected_Implementation(UDataTable* _DataTable, FName _RowName);

	UFUNCTION(BlueprintNativeEvent)
	void PlaySounds(class UFMODEvent* _MelodicSeq, class UFMODEvent* _EffortBankLine);

	virtual void PlaySounds_Implementation(class UFMODEvent* _MelodicSeq, class UFMODEvent* _EffortBankLine);
#pragma endregion

#pragma region Interfaces
	UFUNCTION(BlueprintCallable)
	virtual void StartDialogue(class AHYPlayerCharacterBase* _PlayerRef) override;

	UFUNCTION(BlueprintCallable)
	virtual void EndDialogue(AHYPlayerCharacterBase* _PlayerRef) override;
#pragma endregion

#pragma region Functions
	UFUNCTION(BlueprintCallable)
	void UpdateLore();

	UFUNCTION(BlueprintCallable)
	void ShowReplyOptions(TArray<struct FOptionsDialogue> _ReplyOptions);
#pragma endregion

#pragma region Variables
	// The name of speaker
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	FText Name;

	// Data table to get line
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	UDataTable* DataTable;

	// Row name of data table
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	FName RowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	bool bIsDialogueEnded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	TObjectPtr<class UHYDialogueWidget> DialogueWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	bool bIsEventTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	bool bIsReplyDialogueOpened;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	EDialogueEventType EventTriggerType;
#pragma endregion

#pragma region Resources
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Resources")
	TSubclassOf<class UUserWidget> DialogueWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Resources")
	TSubclassOf<class UUserWidget> DialogueOptionWidgetClass;
#pragma endregion

#pragma region Event Dispatchers
	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnDialogueEventTrigger OnDialogueEventTrigger;
#pragma endregion
};
