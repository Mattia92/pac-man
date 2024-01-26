// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PacManPawn.generated.h"

UCLASS()
class PACMAN_API APacManPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APacManPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

protected:
	bool Frozen = true;
	int32 Lives = 1;

public:
	void SetDirection(const FVector Direction);
	bool IsFrozen() { return Frozen; }
	void SetFrozen(bool Value) { Frozen = Value; }
	int32 GetLives() { return Lives; }
	void HandleDestruction();
	void SpawnEmitterForDuration(float TimeDuration);

private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent *MeshComponent;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent *PacManMeshComponent;
	UPROPERTY(EditAnywhere)
	class UFloatingPawnMovement *PacManFloatingPawnMovement;
	UPROPERTY(EditAnywhere)
	class UParticleSystem *PowerUpParticleSystem;
	UPROPERTY(EditAnywhere)
	float MaxSpeed = 800.f;
	UPROPERTY(EditAnywhere)
	float Acceleration = 1600.f;
	class APacManGameMode *PacManGameMode;
	class UParticleSystemComponent *PowerUpParticleSystemComponent;

	UFUNCTION()
	void OnOverlapBegin(AActor *PlayerActor, AActor *OtherActor);
	UFUNCTION()
	void OnActorHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit);
	void DestroyEmitter();
};
