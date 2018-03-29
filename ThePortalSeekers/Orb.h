// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "Orb.generated.h"

/**
 * Contains the logic of the orb
 * Inherits from the pickup class, so the orb object can be pickedup
 */
UCLASS()
class THEPORTALSEEKERS_API AOrb : public APickup
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrb();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	//Override the GetCollection function - use Implementation because it's a blueprint native event 
	void GetCollectedImplementation() override;


private:
	
	
	
};
