// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PacManGameMode.generated.h"

UCLASS()
class PACMAN_API APacManGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	void ActorEaten(AActor *EatenActor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	class APacManPlayerController *PacManPlayerController;
	class ACameraActor *ViewTargetActor;
	TArray<AActor *> Ghosts;
	float StartDelay = 3.f;
	int32 RegularPickups = 0;

	void HandleGameStart();
	int32 GetRegularPickupCount();
};
