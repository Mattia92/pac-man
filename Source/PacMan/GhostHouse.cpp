// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostHouse.h"
#include "GhostPawn.h"

// Sets default values
AGhostHouse::AGhostHouse()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OnActorBeginOverlap.AddDynamic(this, &AGhostHouse::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AGhostHouse::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGhostHouse::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGhostHouse::OnOverlapBegin(AActor *HouseActor, AActor *OtherActor)
{
	AGhostPawn* Ghost = Cast<AGhostPawn>(OtherActor);
	if (Ghost && Ghost->GetGhostState() == EGhostState::Dead)
	{
		Ghost->RespawnGhost();
	}
}

