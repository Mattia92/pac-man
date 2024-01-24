// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PacManHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class PACMAN_API UPacManHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateText(const FString& UpdatedText);
	UFUNCTION(BlueprintImplementableEvent)
	void SetUpAfterDelay(const float Delay);

	void SetScore(int32 Score);
};
