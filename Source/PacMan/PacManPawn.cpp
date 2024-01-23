// Fill out your copyright notice in the Description page of Project Settings.

#include "PacManPawn.h"
#include "Pickup.h"
#include "PacManGameMode.h"
#include "GhostPawn.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APacManPawn::APacManPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APacManPawn::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &APacManPawn::OnOverlapBegin);
	PacManGameMode = Cast<APacManGameMode>(UGameplayStatics::GetGameMode(this));
}

// Called every frame
void APacManPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Frozen)
	{
		AddMovementInput(GetActorForwardVector());
	}
}

// Called to bind functionality to input
void APacManPawn::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APacManPawn::SetDirection(const FVector Direction)
{
	if (Direction == FVector::UpVector)
	{
		SetActorRotation(FRotator(0, 270, 0));
	}
	else if (Direction == FVector::DownVector)
	{
		SetActorRotation(FRotator(0, 90, 0));
	}
	else if (Direction == FVector::LeftVector)
	{
		SetActorRotation(FRotator(0, 180, 180));
	}
	else if (Direction == FVector::RightVector)
	{
		SetActorRotation(FRotator(0, 0, 0));
	}
}

void APacManPawn::HandleDestruction()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}

void APacManPawn::OnOverlapBegin(AActor *PlayerActor, AActor *OtherActor)
{
	if (PacManGameMode)
	{
		if (APickup *PickUp = Cast<APickup>(OtherActor))
		{
			PacManGameMode->ActorEaten(PickUp);
    	}
    	else if (AGhostPawn *GhostPawn = Cast<AGhostPawn>(OtherActor))
    	{
			if (GhostPawn->GetGhostState() == EGhostState::Frightened)
			{
				PacManGameMode->ActorEaten(GhostPawn);
			}
    	}
	}
}
