// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "PeerInfo.h"
#include "TCPServerConnection.h"
//#include "Engine.h"

/**
* This class takes care of all the communication with the server
* Starts a threads for receiving messages and incorporates the logic to send messages.
* Communicates with the thread class using the inbox queue
* 
*/
class THEPORTALSEEKERS_API Server_Connection
{
public:
	Server_Connection(UWorld* _World);
	~Server_Connection();

	enum GameMessageType {
		OP_NULL = 0,
		OP_PLAY = 1, //asks to play
		OP_ID = 2, //Message contains ID and number of players
		OP_IP = 3, //messages received with this code contain ips of the peers
		OP_INFO = 4, //info messages sent by the server 
		OP_FINISH = 5 //Informs the server that the game has finished
	};

	// To be called in the tick of the actor instanciating this class
	void Tick();
	//Getter Method for the Peer Addresses
	TArray<PeerInfo*> GetPeersInformation();
	//Getter method for the number of players
	int GetNumberPlayer();
	//Getter Method for the player Id
	int GetPlayerID();
	//Checks if the matchmaking is complete
	bool IsMatchmakingComplete();
	//Build and send a message based on the op code
	void SendMessageToServer(int op_code);


private:
	//Creates a message base on the code provided as a parameter
	FString CreateMessage(int OP_Code);
	//Gets Message from the receive Queue
	FString* GetFromInbox();
	//Gets message from the queue and updates the game client
	void DeconstructMessage(TArray<FString> message);
	//Pointer to the game world
	UWorld* World;
	//Inbox Message Queue	
	TQueue<FString*, EQueueMode::Mpsc>* Inbox;
	//Instance of the class connection
	TCPServerConnection* TCPConnection = nullptr;
	//Array containing all the peers information
	TArray<PeerInfo*> PeersInformation;
	//Num of Players returned by the server to be used to load the level 
	int NumPlayers = 0;
	//Id of the player returned by the server to be used by this client
	int PlayerID = 0;
	//States if the matchmaking is completed
	bool m_IsMatchmakingComplete = false;
};


