// Fill out your copyright notice in the Description page of Project Settings.

#include "GhostPawn.h"
#include "PacManPawn.h"
#include "PacManGameMode.h"
#include "WaveManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Sound/SoundCue.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
AGhostPawn::AGhostPawn()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetCollisionProfileName(FName("Ghost"));
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
	GhostDeadMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ghost Dead Mesh Component"));
	GhostDeadMeshComponent->SetCollisionProfileName(FName("NoCollision"));
	GhostDeadMeshComponent->SetVisibility(false);
	GhostDeadMeshComponent->SetupAttachment(RootComponent);
	GhostFloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Ghost Pawn Movement"));
	GhostFloatingPawnMovement->MaxSpeed = MaxSpeed;
	GhostFloatingPawnMovement->Acceleration = Acceleration;
}

// Called when the game starts or when spawned
void AGhostPawn::BeginPlay()
{
	Super::BeginPlay();

	MeshComponent->OnComponentHit.AddDynamic(this, &AGhostPawn::OnActorHit);
	PacManPawn = Cast<APacManPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
	AIGhostController = Cast<AAIController>(GetController());
	PacManGameMode = Cast<APacManGameMode>(UGameplayStatics::GetGameMode(this));
	StartLocation = GetActorLocation();

	if (AIGhostController && GhostBehaviorTree)
	{
		AIGhostController->RunBehaviorTree(GhostBehaviorTree);
		AIGhostBlackboardComponent = AIGhostController->GetBlackboardComponent();
		if (AIGhostBlackboardComponent)
		{
			AIGhostBlackboardComponent->SetValueAsBool("IsPathLooping", true);
		}
	}

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
	GhostDeadMeshComponent->SetVisibility(false);

	if (AIGhostBlackboardComponent)
	{
		AIGhostBlackboardComponent->SetValueAsBool("IsChasing", true);
		AIGhostBlackboardComponent->SetValueAsBool("IsScattering", false);
		AIGhostBlackboardComponent->SetValueAsBool("IsFrightened", false);
		AIGhostBlackboardComponent->SetValueAsBool("IsIdle", false);
		AIGhostBlackboardComponent->SetValueAsBool("IsDead", false);
	}
}

void AGhostPawn::Scatter()
{
	GhostState = EGhostState::Scatter;
	GhostDefaultMeshComponent->SetVisibility(true);
	GhostVulnerableMeshComponent->SetVisibility(false);
	GhostDeadMeshComponent->SetVisibility(false);

	if (AIGhostBlackboardComponent)
	{	
		AIGhostBlackboardComponent->SetValueAsBool("IsChasing", false);
		AIGhostBlackboardComponent->SetValueAsBool("IsScattering", true);
		AIGhostBlackboardComponent->SetValueAsBool("IsFrightened", false);
		AIGhostBlackboardComponent->SetValueAsBool("IsIdle", false);
		AIGhostBlackboardComponent->SetValueAsBool("IsDead", false);
	}
}

void AGhostPawn::Frightened()
{
	PreviousGhostState = GhostState;
	GhostState = EGhostState::Frightened;
	GhostDefaultMeshComponent->SetVisibility(false);
	GhostVulnerableMeshComponent->SetVisibility(true);
	GhostDeadMeshComponent->SetVisibility(false);

	if (AIGhostBlackboardComponent)
	{	
		AIGhostBlackboardComponent->SetValueAsBool("IsChasing", false);
		AIGhostBlackboardComponent->SetValueAsBool("IsScattering", false);
		AIGhostBlackboardComponent->SetValueAsBool("IsFrightened", true);
		AIGhostBlackboardComponent->SetValueAsBool("IsIdle", false);
		AIGhostBlackboardComponent->SetValueAsBool("IsDead", false);
	}
	
	if (WaveManager)
	{		
		EnableFrightenedMode();
		GetWorldTimerManager().SetTimer(GhostFrightenedTimerHandle, this, &AGhostPawn::ResetGhost, WaveManager->FrightenedDuration, false);
	}
}

void AGhostPawn::Idle()
{
	GhostState = EGhostState::Idle;
	PauseAllTimers();

	if (AIGhostBlackboardComponent)
	{
		AIGhostBlackboardComponent->SetValueAsBool("IsIdle", true);
		AIGhostBlackboardComponent->SetValueAsBool("IsChasing", false);
		AIGhostBlackboardComponent->SetValueAsBool("IsScattering", false);
		AIGhostBlackboardComponent->SetValueAsBool("IsFrightened", false);
		AIGhostBlackboardComponent->SetValueAsBool("IsDead", false);
	}
}

void AGhostPawn::Dead()
{
	MeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	GhostState = EGhostState::Dead;
	GhostDefaultMeshComponent->SetVisibility(false);
	GhostVulnerableMeshComponent->SetVisibility(false);
	GhostDeadMeshComponent->SetVisibility(true);

	if (AIGhostBlackboardComponent)
	{	
		AIGhostBlackboardComponent->SetValueAsBool("IsDead", true);
		AIGhostBlackboardComponent->SetValueAsBool("IsChasing", false);
		AIGhostBlackboardComponent->SetValueAsBool("IsScattering", false);
		AIGhostBlackboardComponent->SetValueAsBool("IsFrightened", false);
		AIGhostBlackboardComponent->SetValueAsBool("IsIdle", false);
	}
}

void AGhostPawn::HandleDestruction()
{
	GetWorldTimerManager().ClearTimer(GhostFrightenedTimerHandle);
	Dead();
	UGameplayStatics::PlaySound2D(this, ComsumptionSoundCue);
	GhostFloatingPawnMovement->MaxSpeed = MaxSpeed * 2;
}

void AGhostPawn::OnActorHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
	if (PacManGameMode && (GhostState == EGhostState::Chase || GhostState == EGhostState::Scatter) && OtherActor == PacManPawn)
	{
		PacManGameMode->ActorEaten(OtherActor);
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

void AGhostPawn::RespawnGhost()
{
	MeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);
    GetWorldTimerManager().SetTimer(GhostResetTimerHandle, this, &AGhostPawn::ResetGhost, ResetTime, false);
}

void AGhostPawn::ResetGhostAndPhases()
{
	GhostState = EGhostState::Idle;
	GhostDefaultMeshComponent->SetVisibility(true);
	GhostVulnerableMeshComponent->SetVisibility(false);
	GhostDeadMeshComponent->SetVisibility(false);
	GhostFloatingPawnMovement->MaxSpeed = MaxSpeed;
	SetActorLocation(StartLocation);

	if (PacManGameMode && WaveManager)
	{
		FTimerHandle GhostEnabledTimerHandle;
        GetWorldTimerManager().SetTimer(GhostEnabledTimerHandle, this, &AGhostPawn::StartPhaseOne, PacManGameMode->StartDelay, false);
	}

}

void AGhostPawn::EnableFrightenedMode()
{
	GhostFloatingPawnMovement->MaxSpeed = MaxSpeedFrightened;
	PausePhaseTimers();
}

void AGhostPawn::ResetGhost()
{
	GhostFloatingPawnMovement->MaxSpeed = MaxSpeed;
	switch (PreviousGhostState)
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
	UnPausePhaseTimers();
}

void AGhostPawn::PausePhaseTimers()
{
	if (GetWorldTimerManager().IsTimerActive(GhostScatterPhaseTimerHandle))
	{
		GetWorldTimerManager().PauseTimer(GhostScatterPhaseTimerHandle);
	}

	if (GetWorldTimerManager().IsTimerActive(GhostChasePhaseTimerHandle))
	{
		GetWorldTimerManager().PauseTimer(GhostChasePhaseTimerHandle);
	}
}

void AGhostPawn::PauseFrightenedTimer()
{
	if (GetWorldTimerManager().IsTimerActive(GhostFrightenedTimerHandle))
	{
		GetWorldTimerManager().PauseTimer(GhostFrightenedTimerHandle);
	}
}

void AGhostPawn::PauseResetTimer()
{
	if (GetWorldTimerManager().IsTimerActive(GhostResetTimerHandle))
	{
		GetWorldTimerManager().PauseTimer(GhostResetTimerHandle);
	}
}

void AGhostPawn::PauseAllTimers()
{
	PausePhaseTimers();
	PauseFrightenedTimer();
	PauseResetTimer();
}

void AGhostPawn::UnPausePhaseTimers()
{
	if (GetWorldTimerManager().IsTimerPaused(GhostScatterPhaseTimerHandle))
	{
		GetWorldTimerManager().UnPauseTimer(GhostScatterPhaseTimerHandle);
	}

	if (GetWorldTimerManager().IsTimerPaused(GhostChasePhaseTimerHandle))
	{
		GetWorldTimerManager().UnPauseTimer(GhostChasePhaseTimerHandle);
	}
}
