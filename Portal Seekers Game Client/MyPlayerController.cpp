// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "ThePortalSeekersCharacter.h"
#include "Network/MultiplayerActorTracker.h"
#include "UObject/UObjectIterator.h"
#include "ConfigCacheIni.h"

AMyPlayerController::AMyPlayerController() {
	
	

	

}

AMyPlayerController::~AMyPlayerController()
{
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();	
	/*
	bool hasMatchmaking;
	//Sets the pawn to be controlled by this player	
	GConfig->GetBool(TEXT("/Script/Engine.Network"), TEXT("playerID"), hasMatchmaking, GGameIni);
	//If Matchmaking is active, player ID is set in the multiplayer manager
	if (!hasMatchmaking) {		
		int PlayerId;
		//Sets the pawn to be controlled by this player	
		GConfig->GetInt(TEXT("/Script/Engine.Network"), TEXT("playerID"), PlayerId, GGameIni);
		//SetCurrentPlayer(PlayerId);
	}
	else {
		Possess(nullptr);
	}
	*/
}

void AMyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyPlayerController::SetCurrentPlayer(int ID)
{
	//Loop through all player characters and possess the correct player	
	for (TObjectIterator<AThePortalSeekersCharacter> Itr; Itr; ++Itr)
	{
		//If player with this Id is found
		if (Itr->PlayerId == ID) {			
			//Set Current Player
			CurrentPlayer = *Itr;
			//Control this player
			this->Possess(*Itr);
		}
		//Else create an AI controller for this character
		else {
			Itr->SetAIController();
		}
	}
}

AThePortalSeekersCharacter* AMyPlayerController::GetCurrentPlayer()
{	
	return CurrentPlayer;
}

int* AMyPlayerController::GetCurrentPlayerID()
{
	return &CurrentPlayer->PlayerId;
}

