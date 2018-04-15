// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ThePortalSeekersGameMode.h"
#include "ThePortalSeekersCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "ConfigCacheIni.h"
#include "MyPlayerController.h"
#include "Network/MultiplayerActorTracker.h"
#include "UObject/UObjectIterator.h"
#include "Components/SkeletalMeshComponent.h"


AThePortalSeekersGameMode::AThePortalSeekersGameMode()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	DefaultPawnClass = nullptr;

}

AThePortalSeekersGameMode::~AThePortalSeekersGameMode()
{
	
}

void AThePortalSeekersGameMode::BeginPlay()
{
	Super::BeginPlay();	
	//Checks if multiplayer is enabled on the config file
	int32 hasMultiplayer; //Changed in game ini file;
	GConfig->GetInt(TEXT("/Script/Engine.Network"), TEXT("multiplayer"), hasMultiplayer, GGameIni);
	//If disabled -> all network communication is will not start
	if (hasMultiplayer == 0) {
		return;
	}		
	//Checks if matchmaking is enabled on the config file
	GConfig->GetBool(TEXT("/Script/Engine.Network"), TEXT("matchmaking"), hasMatchmaking, GGameIni);
	//If enabled -> instanciate the server connection class
	if (hasMatchmaking) {
		ServerConnection = new Server_Connection(GetWorld());
	}	
	else {
		// setting this to true will activate the multiplayer instanciation
		bStartGameTrigger = true;
	}
}

void AThePortalSeekersGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
	//If Server Connection has been instanciated -> Call tick of the server connection
	if (ServerConnection != nullptr && !hasGameStarted) {
		// retrives messages from the text box and parses them
		ServerConnection->Tick();
	}	

	//When matchmaking is complete Setup game variables
	if (ServerConnection != nullptr && ServerConnection->IsMatchmakingComplete() && !hasGameStarted) {
		//Get the PLayerController
		AMyPlayerController* Controller = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
		//Retrive Adresses, ID and Num of Players
		PeersInformation = ServerConnection->GetPeersInformation();
		PlayerID = ServerConnection->GetPlayerID();
		NumberPlayers = ServerConnection->GetNumberPlayer();
		
		//IF current controller is not null -> Set current ID for the player and possess character
		if (Controller != nullptr) {
			Controller->SetCurrentPlayer(PlayerID);			
		}
		//If Player ID corresponds to the guardian set this client to authoritative
		if (PlayerID == 0) {
			isAuthoritative = true;
		}
		//Set the number of player characters in-game. Destroys characters that are not being used
		SetNumberOfCharactersPlaying();
		//Trigger Game Start
		bStartGameTrigger = true;
	}

	if (bStartGameTrigger) {
		UE_LOG(LogTemp, Warning, TEXT("Starting Game..."));		
		AMyPlayerController* Controller = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
		//IF current controller != MyController -> Procede
		if (Controller != nullptr) {			
			if (hasMatchmaking) {				
				MultiplayerManager = new Multiplayer(GetWorld(), PeersInformation); //Start Multiplayer Manager Class
			}
			else { //If Matchmaking is turned off get the player ID from the configuration file				
				int32 playerID;
				GConfig->GetInt(TEXT("/Script/Engine.Network"), TEXT("playerID"), playerID, GGameIni);				
				if (playerID == 0) {//If player id the guardian set this client as authoritative
					isAuthoritative = true;
				}							
				if (Controller != nullptr) {
					//IF current controller is not null -> Set current ID for the player and possess character
					Controller->SetCurrentPlayer(playerID);
				}				
				MultiplayerManager = new Multiplayer(GetWorld()); //Start Multiplayer Manager Class		 		
			}	
			//Get the current player character
			AActor* const CurrentPlayer = Cast<AActor>(Controller->GetCurrentPlayer());
			if (CurrentPlayer != nullptr) {
				hasGameStarted = true; //start the game
				MultiplayerManager->Start();  //setup multiplayer
				//Register the player character to send updates to other peers
				Controller->GetCurrentPlayer()->GetMultiplayerActorTracker()->RegisterTrackActorPosition();
			}
			else			
				UE_LOG(LogTemp, Warning, TEXT("Player not found"));			
		}
		bStartGameTrigger = false; //Set trigger to false
	}
	//Update the multiplayer Manager 
	if (MultiplayerManager != nullptr && hasGameStarted) {
		MultiplayerManager->Update();
	}	
}

bool AThePortalSeekersGameMode::HasGameStarted()
{
	return hasGameStarted;
}

void AThePortalSeekersGameMode::GameFinished()
{
	//Communicate with the server to store game statistics
	
	//Send message to other peers

	
}


void AThePortalSeekersGameMode::SetWinner(int winnerValue)
{
	m_winner = winnerValue;
}

int AThePortalSeekersGameMode::GetWinner()
{
	return m_winner;
}

int AThePortalSeekersGameMode::GetPlayerID()
{
	return PlayerID;
}

int AThePortalSeekersGameMode::GetNumberOfPlayers()
{
	return NumberPlayers;
}

void AThePortalSeekersGameMode::SaveTimeStarted()
{
	FDateTime date = FDateTime();
	m_timeStarted = date.GetDate().ToString();
}

FString AThePortalSeekersGameMode::GetTimeStarted()
{
	return m_timeStarted;
}

void AThePortalSeekersGameMode::SetNumberOfCharactersPlaying()
{
	//Loop through all player characters
	for (TObjectIterator<AThePortalSeekersCharacter> Itr; Itr; ++Itr)
	{
		//If player ID is bigger than the number of players
		if (Itr->PlayerId >= NumberPlayers) {
			//Destroy extra characters
			Itr->GetMesh()->SetVisibility(false);
			Itr->SetActorEnableCollision(false);
		}
	}
}



