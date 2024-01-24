// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class PACMAN_API APickup : public AActor
{
	GENERATED_BODY()

public:
	int32 GetPoints() { return Points; }
	void Consume();

protected:
	UPROPERTY(EditAnywhere)
	int32 Points = 10;

private:
	UPROPERTY(EditAnywhere)
	class USoundCue *ComsumptionSoundCue;
};
