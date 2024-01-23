// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveManager.generated.h"

UCLASS()
class PACMAN_API AWaveManager : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float Wave1ScatterDuration = 7.f;
	UPROPERTY(EditAnywhere)
	float Wave1ChaseDuration = 20.f;
	UPROPERTY(EditAnywhere)
	float Wave2ScatterDuration = 7.f;
	UPROPERTY(EditAnywhere)
	float Wave2ChaseDuration = 20.f;
	UPROPERTY(EditAnywhere)
	float Wave3ScatterDuration = 5.f;
	UPROPERTY(EditAnywhere)
	float Wave3ChaseDuration = 20.f;
	UPROPERTY(EditAnywhere)
	float Wave4ScatterDuration = 5.f;
	UPROPERTY(EditAnywhere)
	float FrightenedDuration = 15.f;
};
