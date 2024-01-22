// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GhostPawn.generated.h"

UENUM()
enum class EGhostState : uint8
{
	Chasing,
	Fleeing,
	Idle
};

UCLASS()
class PACMAN_API AGhostPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGhostPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere)
	EGhostState GhostState = EGhostState::Chasing;

	void Hunt();
	void Flee();
	void Idle();

private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent *MeshComponent;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent *GhostDefaultMeshComponent;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent *GhostVulnerableMeshComponent;
	class APacManPawn *PacManPawn;
	class AAIController *AIGhostController;
	class APacManGameMode *PacManGameMode;
	float StartDelay = 3.f;

	UFUNCTION()
	void OnActorHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit);
	void UpdateMovementTarget();
};
