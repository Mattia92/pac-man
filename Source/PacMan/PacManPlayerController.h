// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PacManPlayerController.generated.h"

UCLASS()
class PACMAN_API APacManPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetupInputComponent() override;

public:
	class APacManPawn* GetPacManPawn() { return PacManPawn; }
	void SetPlayerEnabledState(bool bPlayerEnabled);
	void ResetLocationAndRotation();
	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();

private:
	class APacManPawn *PacManPawn;
	FVector StartLocation;
	FRotator StartRotation;
};
