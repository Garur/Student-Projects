// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ThePortalSeekersCharacter.h"
#include "Network/Multiplayer.h"
#include "Network/Server_Connection.h"
#include "ThePortalSeekersGameMode.generated.h"

/**
* Resposible to instanciate the Multiplayer and Server Connection class that initiallize the network functionalities
* Passes the information received from the server through the TCP connection the Multiplayer.cpp
* Runs the tick() of both server_connection and multiplayer classes 
*/
UCLASS(minimalapi)
class AThePortalSeekersGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AThePortalSeekersGameMode();
	~AThePortalSeekersGameMode();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//States if the game has started and active
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	bool HasGameStarted();	
	//Connects to the server to save statistics, to be called by a blueprint (CURRENTLY NOT FULLY IMPLEMENTED)
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void GameFinished();	
	//Getter for Server Connection
	FORCEINLINE class Server_Connection* GetServerConnection() const { return ServerConnection; }
	//Getter for MultiplayerManager
	FORCEINLINE class Multiplayer* GetMultiplayerManager() const { return MultiplayerManager; }	
	//Setter Method for m_winner
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void SetWinner(int winnerValue);
	//Getter For m_winner variable
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	int GetWinner();
	//Getter method for the player Id of this client
	int GetPlayerID();
	//Getter method for the number of players currently in this game session
	int GetNumberOfPlayers();
	//Saves the time the game has started to be later sent to the server when the game is finished
	void SaveTimeStarted();
	//Getter for the time started 
	FString GetTimeStarted();


	//States if the client is the guardian, if true he is authoritative over some mechanics
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Multiplayer")
	bool isAuthoritative = false;	

private:
	// Sets the number of players playing the current game
	void SetNumberOfCharactersPlaying();
	//Instance of the server connection instance
	Server_Connection * ServerConnection = nullptr;
	//Instance of the Multiplayer Manager Class
	Multiplayer* MultiplayerManager = nullptr;
	//Number of players playing the current game
	int32 NumberPlayers = 0;
	//ID of this client player
	int PlayerID = 0;	
	//If true initiate the matchmaking
	bool hasMatchmaking;
	//True if the game has started
	bool hasGameStarted;
	//Used to initiate the multiplayer class and start the game
	bool bStartGameTrigger;
	//Array containing the information of all the peers
	TArray<PeerInfo*> PeersInformation;
	//States who has won the game. Information to be sent to the server when the game ends ( 0 = guardian and 1 = seekers)
	int m_winner;
	//Timestamp that registers the time the game has started
	FString m_timeStarted;
};

enum GameWinner {
	GUARDIAN = 0,
	SEEKERS = 1,
};


