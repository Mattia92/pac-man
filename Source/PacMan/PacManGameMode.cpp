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
                Ghost->Flee();
            }
        }
        
        if (RegularPickups == 0)
        {
            UE_LOG(LogTemp, Display, TEXT("EATEN ALL!!!"));
        }
    }
    else if (APacManPawn *PacManPawn = Cast<APacManPawn>(EatenActor))
    {
        PacManPawn->HandleDestruction();
    }
    
}

void APacManGameMode::HandleGameStart()
{
    RegularPickups = GetRegularPickupCount();

    PacManPlayerController = Cast<APacManPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
    ViewTargetActor = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ACameraActor::StaticClass()));
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
    UE_LOG(LogTemp, Display, TEXT("TOT REG PICK: %s"), *FString::FromInt(Pickups.Num()));
    return Pickups.Num();
}
