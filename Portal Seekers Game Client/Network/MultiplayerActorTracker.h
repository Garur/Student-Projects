// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThePortalSeekersGameMode.h"
#include "Components/ActorComponent.h"
#include "MultiplayerActorTracker.generated.h"


/**
* Component that can be attached to characters that registers events to be sent through the network
* The events are registered in the multiplayer.cpp class
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEPORTALSEEKERS_API UMultiplayerActorTracker : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMultiplayerActorTracker();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//Start Tracking Updates to send to the Multiplayer Manager
	void RegisterTrackActorPosition();
	//Stop Tracking Updates to send to the Multiplayer Manager
	void UnregisterTrackActorPosition();
	//Registers the orb pickup event to send to the Multiplayer Manager
	void RegisterOrbPickup(bool isPickedUp, AActor* Player, AActor* Orb);
	//Returns isBeingTrackedVariable (Getter)
	bool IsBeingTracked();

private:
	// Called when the game starts
	virtual void BeginPlay() override;
	//Reference to the game mode
	AThePortalSeekersGameMode* GameMode;
	//If through -> character is being tracked by multiplayer manager
	bool isBeingTracked;

	
};
