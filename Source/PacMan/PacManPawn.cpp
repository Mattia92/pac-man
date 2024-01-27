// Fill out your copyright notice in the Description page of Project Settings.

#include "PacManPawn.h"
#include "Pickup.h"
#include "PacManGameMode.h"
#include "GhostPawn.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
APacManPawn::APacManPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetCollisionProfileName(FName("PacMan"));
	MeshComponent->SetEnableGravity(false);
	MeshComponent->SetVisibility(false);
	RootComponent = MeshComponent;
	PacManMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PacMan Mesh Component"));
	PacManMeshComponent->SetCollisionProfileName(FName("NoCollision"));
	PacManMeshComponent->SetVisibility(true);
	PacManMeshComponent->SetupAttachment(RootComponent);
	PacManFloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("PacMan Pawn Movement"));
	PacManFloatingPawnMovement->MaxSpeed = MaxSpeed;
	PacManFloatingPawnMovement->Acceleration = Acceleration;
	PacManFloatingPawnMovement->SetPlaneConstraintEnabled(true);
	PacManFloatingPawnMovement->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Z);
}

// Called when the game starts or when spawned
void APacManPawn::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &APacManPawn::OnOverlapBegin);
	MeshComponent->OnComponentHit.AddDynamic(this, &APacManPawn::OnActorHit);
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
	//SetActorHiddenInGame(true);
	//SetActorEnableCollision(false);
	//SetActorTickEnabled(false);
	Lives--;
}

void APacManPawn::OnOverlapBegin(AActor *PlayerActor, AActor *OtherActor)
{
	if (PacManGameMode)
	{
		if (APickup *PickUp = Cast<APickup>(OtherActor))
		{
			PacManGameMode->ActorEaten(PickUp);
    	}
	}
}

void APacManPawn::OnActorHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
	AGhostPawn* GhostPawn = Cast<AGhostPawn>(OtherActor);
	if (PacManGameMode && GhostPawn && GhostPawn->GetGhostState() == EGhostState::Frightened)
	{
		PacManGameMode->ActorEaten(OtherActor);
	}
}

void APacManPawn::SpawnEmitterForDuration(float TimeDuration)
{
	FTimerHandle PowerUpParticleSystemTimerHandle;
	PowerUpParticleSystemComponent = UGameplayStatics::SpawnEmitterAttached(PowerUpParticleSystem, GetRootComponent());
    GetWorldTimerManager().SetTimer(PowerUpParticleSystemTimerHandle, this, &APacManPawn::DestroyEmitter, TimeDuration, false);
}

void APacManPawn::DestroyEmitter()
{
	PowerUpParticleSystemComponent->DeactivateSystem();
}
