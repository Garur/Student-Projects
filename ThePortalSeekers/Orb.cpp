// Fill out your copyright notice in the Description page of Project Settings.

#include "Orb.h"
#include "ThePortalSeekers.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AOrb::AOrb()
{
	GetMesh()->SetSimulatePhysics(true);

}

void AOrb::BeginPlay()
{
}

void AOrb::Tick(float DeltaTime)
{
}

void AOrb::GetCollectedImplementation() {
	//Use base pickup behaviour
	Super::GetCollected_Implementation();
	//Destroy the battery
	//Destroy();
}



