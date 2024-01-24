// Fill out your copyright notice in the Description page of Project Settings.


#include "PacManHUDWidget.h"

void UPacManHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UPacManHUDWidget::SetScore(int32 Score)
{
    UpdateText(FString::FromInt(Score));
}

