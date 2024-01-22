// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Teleporter.generated.h"

UCLASS()
class PACMAN_API ATeleporter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeleporter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere)
	ATeleporter* Target;
	UPROPERTY(EditAnywhere)
	class USoundCue* TeleportSoundCue;

	void TeleportToTarget(AActor* Actor);

private:
	UFUNCTION()
	void OnOverlapBegin(AActor *TeleporterActor, AActor *OtherActor);
};
