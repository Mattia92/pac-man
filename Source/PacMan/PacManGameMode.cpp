// Fill out your copyright notice in the Description page of Project Settings.

#include "PacManGameMode.h"
#include "PacManPlayerController.h"
#include "Pickup.h"
#include "PacManPawn.h"
#include "GhostPawn.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"

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
        
        if (RegularPickups == 0)
        {
            //SET WIN
            UE_LOG(LogTemp, Display, TEXT("EATEN ALL!!!"));
        }
    }
    else if (APacManPawn *PacManPawn = Cast<APacManPawn>(EatenActor))
    {
        PacManPawn->HandleDestruction();
        //RESET ALL
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

    if (PacManPlayerController && ViewTargetActor)
    {
        PacManPlayerController->SetPlayerEnabledState(false);
        PacManPlayerController->SetViewTarget(ViewTargetActor);

        FTimerHandle PlayerEnabledTimerHandle;
        FTimerDelegate PlayerEnabledTimerDelegate = FTimerDelegate::CreateUObject(PacManPlayerController, &APacManPlayerController::SetPlayerEnabledState, true);
        GetWorldTimerManager().SetTimer(PlayerEnabledTimerHandle, PlayerEnabledTimerDelegate, StartDelay, false);
    }
    
}

int32 APacManGameMode::GetRegularPickupCount()
{
    TArray<AActor *> Pickups;
    UGameplayStatics::GetAllActorsOfClassWithTag(this, APickup::StaticClass(), "Regular", Pickups);
    return Pickups.Num();
}
