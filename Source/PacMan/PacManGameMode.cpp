// Fill out your copyright notice in the Description page of Project Settings.

#include "PacManGameMode.h"
#include "PacManPlayerController.h"
#include "Pickup.h"
#include "PacManPawn.h"
#include "GhostPawn.h"
#include "PacManHUDWidget.h"
#include "PacManEndGameWidget.h"
#include "WaveManager.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

// Called when the game starts or when spawned
void APacManGameMode::BeginPlay()
{
    Super::BeginPlay();

    HandleGameStart();
}

void APacManGameMode::ActorEaten(AActor *EatenActor)
{
    if (APickup *EatenPickup = Cast<APickup>(EatenActor))
    {
        EatenPickup->Consume();
        if (EatenPickup->ActorHasTag("Regular"))
        {
            RegularPickups--;
        }
        else if (EatenPickup->ActorHasTag("PowerUp"))
        {
            if (PacManPlayerController && PacManPlayerController->GetPacManPawn() && WaveManager)
            {
                PacManPlayerController->GetPacManPawn()->SpawnEmitterForDuration(WaveManager->FrightenedDuration);
            }

            for (AActor *Actor : Ghosts)
            {
                AGhostPawn *Ghost = Cast<AGhostPawn>(Actor);
                Ghost->Frightened();
            }
        }

        if (PacManHUDWidget)
        {
            PacManHUDWidget->SetScore(CurrentScore += EatenPickup->GetPoints());
        }

        if (EndGameWidgetClass && RegularPickups == 0)
        {
            GameplayAudioComponent->SetSound(EndGameWinSoundCue);
            PacManEndGameWidget->AddToViewport();
            PacManEndGameWidget->GameOver(true);
        }
    }
    else if (APacManPawn *PacManPawn = Cast<APacManPawn>(EatenActor))
    {
        for (AActor *Actor : Ghosts)
        {
            AGhostPawn *Ghost = Cast<AGhostPawn>(Actor);
            if (Ghost)
            {
                Ghost->Idle();
            }
        }

        if (PacManPlayerController)
        {
            PacManPawn->HandleDestruction();
        }

        if (PacManHUDWidget)
        {
            PacManHUDWidget->UpdateLives(PacManPlayerController->GetPacManPawn()->GetLives());
        }

        if (EndGameWidgetClass && PacManPlayerController->GetPacManPawn()->GetLives() <= 0)
        {
            GameplayAudioComponent->SetSound(EndGameLoseSoundCue);
            PacManEndGameWidget->AddToViewport();
            PacManEndGameWidget->GameOver(false);
        }
        else
        {
            if (DeathAudioComponent)
            {
                DeathAudioComponent->SetSound(DeathSoundCue);
                DeathAudioComponent->Play();
            }
        }
    }
    else if (AGhostPawn *GhostPawn = Cast<AGhostPawn>(EatenActor))
    {
        GhostPawn->HandleDestruction();
    }
}

void APacManGameMode::HandleGameStart()
{
    RegularPickups = GetRegularPickupCount();

    PacManPlayerController = Cast<APacManPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
    ViewTargetActor = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(this, ACameraActor::StaticClass()));
    WaveManager = Cast<AWaveManager>(UGameplayStatics::GetActorOfClass(this, AWaveManager::StaticClass()));
    UGameplayStatics::GetAllActorsOfClass(this, AGhostPawn::StaticClass(), Ghosts);

    if (IsValid(HUDWidgetClass))
    {
        if (!PacManHUDWidget)
        {
            PacManHUDWidget = Cast<UPacManHUDWidget>(CreateWidget(GetWorld(), HUDWidgetClass));
        }

        if (PacManHUDWidget)
        {
            if (!PacManHUDWidget->IsInViewport())
            {
                PacManHUDWidget->AddToViewport();
            }
            PacManHUDWidget->SetUpAfterDelay(StartDelay);
        }
    }

    if (IsValid(EndGameWidgetClass) && !PacManEndGameWidget)
    {
        PacManEndGameWidget = Cast<UPacManEndGameWidget>(CreateWidget(GetWorld(), EndGameWidgetClass));
    }

    if (PacManPlayerController && ViewTargetActor)
    {
        PacManPlayerController->SetPlayerEnabledState(false);
        PacManPlayerController->SetViewTarget(ViewTargetActor);

        FTimerHandle PlayerEnabledTimerHandle;
        FTimerDelegate PlayerEnabledTimerDelegate = FTimerDelegate::CreateUObject(PacManPlayerController, &APacManPlayerController::SetPlayerEnabledState, true);
        GetWorldTimerManager().SetTimer(PlayerEnabledTimerHandle, PlayerEnabledTimerDelegate, StartDelay, false);
    }

    if (GameplaySoundCue && !GameplayAudioComponent)
    {
        GameplayAudioComponent = UGameplayStatics::SpawnSound2D(this, GameplaySoundCue);
    }

    if (DeathSoundCue && !DeathAudioComponent)
    {
        DeathAudioComponent = UGameplayStatics::SpawnSound2D(this, DeathSoundCue);
        DeathAudioComponent->Stop();
        if (DeathAudioComponent)
        {
            DeathAudioComponent->OnAudioFinished.AddDynamic(this, &APacManGameMode::HandleGameReset);
        }
    }
}

void APacManGameMode::HandleGameReset()
{
    if (PacManPlayerController)
    {
        PacManPlayerController->ResetLocationAndRotation();
        for (AActor *Actor : Ghosts)
        {
            AGhostPawn *Ghost = Cast<AGhostPawn>(Actor);
            if (Ghost)
            {
                Ghost->ResetGhostAndPhases();
            }
        }
        HandleGameStart();
    }
}

int32 APacManGameMode::GetRegularPickupCount()
{
    TArray<AActor *> Pickups;
    UGameplayStatics::GetAllActorsOfClassWithTag(this, APickup::StaticClass(), "Regular", Pickups);
    return Pickups.Num();
}
