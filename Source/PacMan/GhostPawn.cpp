// Fill out your copyright notice in the Description page of Project Settings.

#include "GhostPawn.h"
#include "PacManPawn.h"
#include "PacManGameMode.h"
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

	FTimerHandle GhostEnabledTimerHandle;
	Idle();
	GetWorldTimerManager().SetTimer(GhostEnabledTimerHandle, this, &AGhostPawn::Hunt, StartDelay, false);
}

void AGhostPawn::Hunt()
{
	GhostState = EGhostState::Chasing;
	GhostDefaultMeshComponent->SetVisibility(true);
	GhostVulnerableMeshComponent->SetVisibility(false);
	AIGhostBlackboardComponent->SetValueAsBool("IsChasing", true);
	AIGhostBlackboardComponent->SetValueAsBool("IsIdle", false);
}

void AGhostPawn::Flee()
{
	GhostState = EGhostState::Fleeing;
	GhostDefaultMeshComponent->SetVisibility(false);
	GhostVulnerableMeshComponent->SetVisibility(true);
	AIGhostBlackboardComponent->SetValueAsBool("IsChasing", false);
	AIGhostBlackboardComponent->SetValueAsBool("IsIdle", false);
}

void AGhostPawn::Idle()
{
	GhostState = EGhostState::Idle;
	AIGhostBlackboardComponent->SetValueAsBool("IsIdle", true);
}

void AGhostPawn::OnActorHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
	if (PacManGameMode && GhostState == EGhostState::Chasing && OtherActor == PacManPawn)
	{
		PacManGameMode->ActorEaten(OtherActor);
		Idle();
	}
}
