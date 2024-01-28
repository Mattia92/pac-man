// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PacManGameMode.generated.h"

UCLASS()
class PACMAN_API APacManGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	float StartDelay = 3.f;

	void ActorEaten(AActor *EatenActor);

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> HUDWidgetClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> EndGameWidgetClass;
	UPROPERTY(VisibleInstanceOnly)
	class UPacManHUDWidget *PacManHUDWidget;
	UPROPERTY(VisibleInstanceOnly)
	class UPacManEndGameWidget *PacManEndGameWidget;

private:
	UPROPERTY(EditAnywhere)
	class USoundCue *GameplaySoundCue;
	UPROPERTY(EditAnywhere)
	class USoundCue *EndGameWinSoundCue;
	UPROPERTY(EditAnywhere)
	class USoundCue *EndGameLoseSoundCue;
	UPROPERTY(EditAnywhere)
	class USoundCue *DeathSoundCue;
	UPROPERTY(VisibleInstanceOnly)
	class UAudioComponent *GameplayAudioComponent;
	UPROPERTY(VisibleInstanceOnly)
	class UAudioComponent *DeathAudioComponent;
	class APacManPlayerController *PacManPlayerController;
	class ACameraActor *ViewTargetActor;
	class AWaveManager *WaveManager;
	TArray<AActor *> Ghosts;
	int32 CurrentScore = 0;
	int32 RegularPickups = 0;

	UFUNCTION()
	void HandleGameReset();
	void HandleGameStart();
	void IncreaseScore(int32 ScoreAmount);
	void DeactivateEmitter();
	int32 GetRegularPickupCount();
};
