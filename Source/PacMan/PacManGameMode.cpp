// Fill out your copyright notice in the Description page of Project Settings.

#include "PacManGameMode.h"
#include "PacManPlayerController.h"
#include "Pickup.h"
#include "PacManPawn.h"
#include "GhostPawn.h"
#include "PacManHUDWidget.h"
#include "PacManEndGameWidget.h"
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
            for(AActor* Actor : Ghosts)
            {
                AGhostPawn* Ghost = Cast<AGhostPawn>(Actor);
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
        PacManPawn->HandleDestruction();
        if (EndGameWidgetClass && PacManPawn->GetLives() <= 0)
        {
            GameplayAudioComponent->SetSound(EndGameLoseSoundCue);
            PacManEndGameWidget->AddToViewport();
            PacManEndGameWidget->GameOver(false);
        }
        else
        {
            //RESET ALL
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
    UGameplayStatics::GetAllActorsOfClass(this, AGhostPawn::StaticClass(), Ghosts);

    if (IsValid(HUDWidgetClass))
    {
        PacManHUDWidget = Cast<UPacManHUDWidget>(CreateWidget(GetWorld(), HUDWidgetClass));
        if (PacManHUDWidget)
        {
            PacManHUDWidget->AddToViewport();
            PacManHUDWidget->SetUpAfterDelay(StartDelay);
        }
    }

    if (IsValid(EndGameWidgetClass))
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

    if (GameplaySoundCue)
    {
	    GameplayAudioComponent = UGameplayStatics::SpawnSound2D(this, GameplaySoundCue);
    }
    
    
}

int32 APacManGameMode::GetRegularPickupCount()
{
    TArray<AActor *> Pickups;
    UGameplayStatics::GetAllActorsOfClassWithTag(this, APickup::StaticClass(), "Regular", Pickups);
    return Pickups.Num();
}
