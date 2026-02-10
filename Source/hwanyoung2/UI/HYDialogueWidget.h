// Copyright 2024. TEAM DAON. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HYDialogueWidget.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API UHYDialogueWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
#pragma region EventGraphs
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable)
	void UpdateDialogue(FText _inText, FText _SpeakerName, UTexture2D* _SpeakerPortrait);

	UFUNCTION(BlueprintCallable)
	void ReplyingEnd();

	UFUNCTION(BlueprintCallable)
	void ReplyingTime();

	UFUNCTION(BlueprintCallable)
	void NextButtonClick();

	UFUNCTION(BlueprintCallable)
	void SkipButtonClick();

	UFUNCTION(BlueprintCallable)
	void Init(FText _DialogueText, FText _SpeakerName, UTexture2D* _SpeakerPortrait, class UHYDialogueComponent* _DialogueComp);
#pragma endregion
	
public:
#pragma region Widget Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Widget|Components")
	TObjectPtr<class UTextBlock> DialogueTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Widget|Components")
	TObjectPtr<class UTextBlock> SpeakerText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Widget|Components")
	TObjectPtr<class UImage> SpeakerTexture;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Widget|Components")
	TObjectPtr<class UScrollBox> OptionScrollBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Widget|Components")
	TObjectPtr<class UButton> SkipButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Widget|Components")
	TObjectPtr<class UButton> NextButton;

#pragma endregion

#pragma region Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExposeOnSpawn")
	FText DialogueText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExposeOnSpawn")
	FText SpeakerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExposeOnSpawn")
	UTexture2D* SpeakerPortrait;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UHYDialogueComponent* DialogueComponent;
#pragma endregion
};
