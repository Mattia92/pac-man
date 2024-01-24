// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

void APickup::Consume()
{
	UGameplayStatics::PlaySound2D(this, ComsumptionSoundCue);
	Destroy();
}

