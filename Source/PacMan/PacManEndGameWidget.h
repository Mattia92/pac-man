// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PacManEndGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class PACMAN_API UPacManEndGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent)
	void GameOver(bool bWonGame);
};
