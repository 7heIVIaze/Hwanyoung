// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HYUIManager.generated.h"

/**
 * 
 */
UCLASS()
class HWANYOUNG2_API UHYUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UHYUIManager();
	
public:
	UFUNCTION(BlueprintCallable)
	void PushUI(class UUserWidget* Widget);

	UFUNCTION(BlueprintCallable)
	void PopUI(class UUserWidget* Widget);

	UFUNCTION(BlueprintCallable)
	class UUserWidget* Top();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TArray<class UUserWidget*> UIStack;
};
