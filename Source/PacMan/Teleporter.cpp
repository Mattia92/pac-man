// Fill out your copyright notice in the Description page of Project Settings.


#include "Teleporter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ATeleporter::ATeleporter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OnActorBeginOverlap.AddDynamic(this, &ATeleporter::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ATeleporter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATeleporter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATeleporter::TeleportToTarget(AActor* Actor)
{
	USceneComponent* TargetSpawn = Cast<USceneComponent>(Target->GetDefaultSubobjectByName("Spawn"));
	UGameplayStatics::PlaySound2D(this, TeleportSoundCue);
	Actor->SetActorLocation(TargetSpawn->GetComponentLocation());
}

void ATeleporter::OnOverlapBegin(AActor *TeleporterActor, AActor *OtherActor)
{
	if (OtherActor->ActorHasTag("PacMan"))
	{
		GetWorldTimerManager().SetTimerForNextTick([OtherActor, this]() { TeleportToTarget(OtherActor); });
	}
	
}

