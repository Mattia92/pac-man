// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GhostPawn.generated.h"

UENUM()
enum class EGhostState : uint8
{
	Chase,
	Scatter,
	Frightened,
	Idle,
	Dead
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

protected:
	EGhostState GhostState = EGhostState::Idle;

public:
	EGhostState GetGhostState() { return GhostState; }
	void Chase();
	void Scatter();
	void Frightened();
	void Idle();
	void Dead();
	void HandleDestruction();
	void RespawnGhost();

private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent *MeshComponent;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent *GhostDefaultMeshComponent;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent *GhostVulnerableMeshComponent;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent *GhostDeadMeshComponent;
	UPROPERTY(EditAnywhere)
	class UFloatingPawnMovement *GhostFloatingPawnMovement;
	UPROPERTY(EditAnywhere)
	class AWaveManager *WaveManager;
	UPROPERTY(EditAnywhere)
	class USoundCue *ComsumptionSoundCue;
	UPROPERTY(EditAnywhere)
	float MaxSpeed = 500.f;
	UPROPERTY(EditAnywhere)
	float MaxSpeedFrightened = 250.f;
	UPROPERTY(EditAnywhere)
	float Acceleration = 4000.f;
	UPROPERTY(EditAnywhere)
	float ResetTime = 5.f;

	class APacManPawn *PacManPawn;
	class AAIController *AIGhostController;
	class UBlackboardComponent *AIGhostBlackboardComponent;
	class APacManGameMode *PacManGameMode;
	FTimerHandle GhostScatterPhaseTimerHandle;
	FTimerHandle GhostChasePhaseTimerHandle;
	FTimerHandle GhostFrightenedTimerHandle;
	EGhostState PreviousGhostState;

	UFUNCTION()
	void OnActorHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit);
	void StartPhaseOne();
	void StartPhaseTwo();
	void StartPhaseThree();
	void StartPhaseFour();
	void ResetGhost();
	void EnableFrightenedMode();
};
