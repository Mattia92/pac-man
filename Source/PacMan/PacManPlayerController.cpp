// Fill out your copyright notice in the Description page of Project Settings.


#include "PacManPlayerController.h"
#include "PacManPawn.h"
#include "Kismet/KismetMathLibrary.h"

// Called when the game starts or when spawned
void APacManPlayerController::BeginPlay()
{
	Super::BeginPlay();

	PacManPawn = Cast<APacManPawn>(GetPawn());
    if (PacManPawn)
    {
        StartLocation = PacManPawn->GetActorLocation();
        StartRotation = PacManPawn->GetActorRotation();
    }
    
}

void APacManPlayerController::SetPlayerEnabledState(bool bPlayerEnabled)
{
    if (PacManPawn)
    {
        if (bPlayerEnabled)
        {
            PacManPawn->EnableInput(this);
        }
        else
        {
            PacManPawn->DisableInput(this);
        }
        SetIgnoreMoveInput(!bPlayerEnabled);
        PacManPawn->SetFrozen(!bPlayerEnabled);
        PacManPawn->SetMovementScale(UKismetMathLibrary::Conv_BoolToFloat(bPlayerEnabled));
    }
}

void APacManPlayerController::ResetLocationAndRotation()
{
    if (PacManPawn)
    {
        PacManPawn->SetActorLocationAndRotation(StartLocation, StartRotation);
        PacManPawn->SetActorEnableCollision(true);
    }
}

void APacManPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAction("MoveUp", IE_Pressed, this, &APacManPlayerController::MoveUp);
    InputComponent->BindAction("MoveDown", IE_Pressed, this, &APacManPlayerController::MoveDown);
    InputComponent->BindAction("MoveLeft", IE_Pressed, this, &APacManPlayerController::MoveLeft);
    InputComponent->BindAction("MoveRight", IE_Pressed, this, &APacManPlayerController::MoveRight);
}

void APacManPlayerController::MoveUp()
{
    if (PacManPawn && !PacManPawn->IsFrozen())
    {
        PacManPawn->SetDirection(FVector::UpVector);
    }
}

void APacManPlayerController::MoveDown()
{
    if (PacManPawn && !PacManPawn->IsFrozen())
    {
        PacManPawn->SetDirection(FVector::DownVector);
    }
}

void APacManPlayerController::MoveLeft()
{
    if (PacManPawn && !PacManPawn->IsFrozen())
    {
        PacManPawn->SetDirection(FVector::LeftVector);
    }
}

void APacManPlayerController::MoveRight()
{
    if (PacManPawn && !PacManPawn->IsFrozen())
    {
        PacManPawn->SetDirection(FVector::RightVector);
    }
}
