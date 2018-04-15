// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerActorTracker.h"
#include "ThePortalSeekersGameMode.h"
#include "ThePortalSeekersCharacter.h"


// Sets default values for this component's properties
UMultiplayerActorTracker::UMultiplayerActorTracker()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	
}


// Called when the game starts
void UMultiplayerActorTracker::BeginPlay()
{
	Super::BeginPlay();
	
	ACharacter* const Player = Cast<ACharacter>(this->GetOwner());
	if (Player != nullptr)
	{
		//if it is a player character do not register this actor
		// The player Controller registers the actor
	}
	else {
		RegisterTrackActorPosition();
	}
	
}

void UMultiplayerActorTracker::RegisterTrackActorPosition()
{	
	//Set Gamemode 	
	if (GameMode == nullptr) {
		GameMode = (AThePortalSeekersGameMode*) this->GetOwner()->GetWorld()->GetAuthGameMode();
	}
	GameMode->GetMultiplayerManager()->RegisterActor(this->GetOwner());
	isBeingTracked = true;
}

void UMultiplayerActorTracker::UnregisterTrackActorPosition()
{
	//Set Gamemode 	
	if (GameMode == nullptr) {
		GameMode = (AThePortalSeekersGameMode*) this->GetOwner()->GetWorld()->GetAuthGameMode();
	}
	GameMode->GetMultiplayerManager()->UnregisterActor(this->GetOwner());
	isBeingTracked = false;
}


// Called every frame
void UMultiplayerActorTracker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMultiplayerActorTracker::RegisterOrbPickup(bool isPickedUp, AActor* Player, AActor* Orb) {
	//Set Gamemode 
	if (GameMode == nullptr) {
		GameMode = (AThePortalSeekersGameMode*) this->GetOwner()->GetWorld()->GetAuthGameMode();		
	}
	GameMode->GetMultiplayerManager()->RegisterOrbPickup(isPickedUp, Player, Orb);
	UE_LOG(LogTemp, Error, TEXT("Register Orb Event"));	
}

bool UMultiplayerActorTracker::IsBeingTracked()
{
	return isBeingTracked;
}

