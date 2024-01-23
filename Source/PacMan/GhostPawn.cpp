// Fill out your copyright notice in the Description page of Project Settings.

#include "GhostPawn.h"
#include "PacManPawn.h"
#include "PacManGameMode.h"
#include "WaveManager.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
AGhostPawn::AGhostPawn()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetCollisionProfileName(FName("Pawn"));
	MeshComponent->SetEnableGravity(false);
	MeshComponent->SetVisibility(false);
	RootComponent = MeshComponent;
	GhostDefaultMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ghost Default Mesh Component"));
	GhostDefaultMeshComponent->SetCollisionProfileName(FName("NoCollision"));
	GhostDefaultMeshComponent->SetVisibility(true);
	GhostDefaultMeshComponent->SetupAttachment(RootComponent);
	GhostVulnerableMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ghost Vulnerable Mesh Component"));
	GhostVulnerableMeshComponent->SetCollisionProfileName(FName("NoCollision"));
	GhostVulnerableMeshComponent->SetVisibility(false);
	GhostVulnerableMeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AGhostPawn::BeginPlay()
{
	Super::BeginPlay();

	MeshComponent->OnComponentHit.AddDynamic(this, &AGhostPawn::OnActorHit);
	PacManPawn = Cast<APacManPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
	AIGhostController = Cast<AAIController>(GetController());
	AIGhostBlackboardComponent = AIGhostController->GetBlackboardComponent();
	PacManGameMode = Cast<APacManGameMode>(UGameplayStatics::GetGameMode(this));
	Idle();

	if (PacManGameMode && WaveManager)
	{
		FTimerHandle GhostEnabledTimerHandle;
        GetWorldTimerManager().SetTimer(GhostEnabledTimerHandle, this, &AGhostPawn::StartPhaseOne, PacManGameMode->StartDelay, false);
	}
}

void AGhostPawn::Chase()
{
	GhostState = EGhostState::Chase;
	GhostDefaultMeshComponent->SetVisibility(true);
	GhostVulnerableMeshComponent->SetVisibility(false);
	AIGhostBlackboardComponent->SetValueAsBool("IsChasing", true);
	AIGhostBlackboardComponent->SetValueAsBool("IsScattering", false);
	AIGhostBlackboardComponent->SetValueAsBool("IsFrightened", false);
	AIGhostBlackboardComponent->SetValueAsBool("IsIdle", false);
}

void AGhostPawn::Scatter()
{
	GhostState = EGhostState::Scatter;
	GhostDefaultMeshComponent->SetVisibility(true);
	GhostVulnerableMeshComponent->SetVisibility(false);
	AIGhostBlackboardComponent->SetValueAsBool("IsChasing", false);
	AIGhostBlackboardComponent->SetValueAsBool("IsScattering", true);
	AIGhostBlackboardComponent->SetValueAsBool("IsFrightened", false);
	AIGhostBlackboardComponent->SetValueAsBool("IsIdle", false);
}

void AGhostPawn::Frightened()
{
	EGhostState OldGhostState = GhostState;
	GhostState = EGhostState::Frightened;
	GhostDefaultMeshComponent->SetVisibility(false);
	GhostVulnerableMeshComponent->SetVisibility(true);
	AIGhostBlackboardComponent->SetValueAsBool("IsChasing", false);
	AIGhostBlackboardComponent->SetValueAsBool("IsScattering", false);
	AIGhostBlackboardComponent->SetValueAsBool("IsFrightened", true);
	AIGhostBlackboardComponent->SetValueAsBool("IsIdle", false);
	
	if (WaveManager)
	{
		FTimerHandle GhostFrightenedTimerHandle;
		FTimerDelegate GhostFrightenedTimerDelegate = FTimerDelegate::CreateUObject(this, &AGhostPawn::EndFrightenedMode, OldGhostState);
		StartFrightenedMode();
		GetWorldTimerManager().SetTimer(GhostFrightenedTimerHandle,GhostFrightenedTimerDelegate, WaveManager->FrightenedDuration, false);
	}
}

void AGhostPawn::Idle()
{
	GhostState = EGhostState::Idle;
	AIGhostBlackboardComponent->SetValueAsBool("IsIdle", true);
	AIGhostBlackboardComponent->SetValueAsBool("IsChasing", false);
	AIGhostBlackboardComponent->SetValueAsBool("IsScattering", false);
	AIGhostBlackboardComponent->SetValueAsBool("IsFrightened", false);
}

void AGhostPawn::OnActorHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
	if (PacManGameMode && (GhostState == EGhostState::Chase || GhostState == EGhostState::Scatter) && OtherActor == PacManPawn)
	{
		PacManGameMode->ActorEaten(OtherActor);
		Idle();
	}
}

void AGhostPawn::StartPhaseOne()
{
	Scatter();
	GetWorldTimerManager().SetTimer(GhostScatterPhaseTimerHandle, this, &AGhostPawn::Chase, WaveManager->Wave1ScatterDuration, false);
	GetWorldTimerManager().SetTimer(GhostChasePhaseTimerHandle, this, &AGhostPawn::StartPhaseTwo, WaveManager->Wave1ScatterDuration + WaveManager->Wave1ChaseDuration, false);
}

void AGhostPawn::StartPhaseTwo()
{
	Scatter();
	GetWorldTimerManager().SetTimer(GhostScatterPhaseTimerHandle, this, &AGhostPawn::Chase, WaveManager->Wave2ScatterDuration, false);
	GetWorldTimerManager().SetTimer(GhostChasePhaseTimerHandle, this, &AGhostPawn::StartPhaseThree, WaveManager->Wave2ScatterDuration + WaveManager->Wave2ChaseDuration, false);
}

void AGhostPawn::StartPhaseThree()
{
	Scatter();
	GetWorldTimerManager().SetTimer(GhostScatterPhaseTimerHandle, this, &AGhostPawn::Chase, WaveManager->Wave3ScatterDuration, false);
	GetWorldTimerManager().SetTimer(GhostChasePhaseTimerHandle, this, &AGhostPawn::StartPhaseFour, WaveManager->Wave3ScatterDuration + WaveManager->Wave3ChaseDuration, false);
}

void AGhostPawn::StartPhaseFour()
{
	Scatter();
	GetWorldTimerManager().SetTimer(GhostScatterPhaseTimerHandle, this, &AGhostPawn::Chase, WaveManager->Wave4ScatterDuration, false);
}

void AGhostPawn::StartFrightenedMode()
{
	//TODO:Settare velocità ridotta
	if (GetWorldTimerManager().IsTimerActive(GhostScatterPhaseTimerHandle))
	{
		GetWorldTimerManager().PauseTimer(GhostScatterPhaseTimerHandle);
	}

	if (GetWorldTimerManager().IsTimerActive(GhostChasePhaseTimerHandle))
	{
		GetWorldTimerManager().PauseTimer(GhostChasePhaseTimerHandle);
	}
}

void AGhostPawn::EndFrightenedMode(EGhostState GhostState)
{
	//TODO:Settare velocità normale
	switch (GhostState)
	{
	case EGhostState::Chase:
		Chase();
		break;
	case EGhostState::Scatter:
		Scatter();
		break;
	default:
		break;
	}

	if (GetWorldTimerManager().IsTimerPaused(GhostScatterPhaseTimerHandle))
	{
		GetWorldTimerManager().UnPauseTimer(GhostScatterPhaseTimerHandle);
	}

	if (GetWorldTimerManager().IsTimerPaused(GhostChasePhaseTimerHandle))
	{
		GetWorldTimerManager().UnPauseTimer(GhostChasePhaseTimerHandle);
	}
}
