// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UDPThreadReceiver.h"
#include "UDPThreadSender.h"
#include "Containers/Queue.h"
#include "GameFramework/Actor.h"
#include "Containers/Array.h"
#include "ThePortalSeekersCharacter.h"
#include <condition_variable>
#include <mutex>
#include "PeerInfo.h"
#include "GameSynchronization.h"

//The Max number of arguments that the multiplayer messages can have
#define MAX_NUM_MSG_ARG 15

/** 
 * This class takes care of all the P2P communication, is the main point of connection 
 * between the network and the game loop.
 * Starts 2 threads one for receiving messages and the other to send messages. 
 * Both threads communicate with this class using the inbox and outbox queue.
 * Responsible to create messages reflecting the current state of the game 
 * and save it in the outbox to be broadcast to other peers.
 * Responsible to deconstruct messages received from the network and update the game
 */
class THEPORTALSEEKERS_API Multiplayer
{
public:
	//Simpler Constructor for when the matchmaking is disabled
	Multiplayer(UWorld* _World);
	//Constructor for when the matchmaking is enabled
	Multiplayer(UWorld* _World, TArray<PeerInfo*> peersAddresses);	
	~Multiplayer();

	//Operation Codes for all the messages
	enum GameMessageType {
		OP_NULL = 0,
		OP_DIE = 1, //states when a player dies (has a Transform)
		OP_RES = 2, //states when a player is ressurected (has Transform)
		OP_RES_START = 3, // sent when a player starts resurecting another player (Transform)
		OP_RES_FINISH = 4, // sent when a player successfully finishes resurecting another player (Transform + player ID?)
		OP_POSITION = 5, //can be used for the position of the players ( Location + Rotator)
		OP_ORB = 6, // states when a orb is picked up or dropped (bool, Player ID, PlayerPosition, Orb ID)  
		OP_PILLAR_START = 7, //sent when a player starts charging a pillar
		OP_PILLAR_FINISH = 8, //sent when a player successfully finishes charging a pillar
		OP_PORTAL = 9, // states when the portal is opened
		OP_ORB_SPAWN = 10, // states when a orb is spawn
		OP_START_GAME = 11, //Serves to the other peers to start the game
		OP_TEST = 12 //Used to test the connections between the peers
	};
	//Register Actor on Multiplayer Updates System (Actor state starts being tracked)
	void RegisterActor(AActor* ActorToRegister);
	//Unregister Actor on Multiplayer Updates System (Actor state stops being tracked)
	void UnregisterActor(AActor* ActorToUnregister);
	//Register OrbPickup
	void RegisterOrbPickup(bool isPickup, AActor* Player, AActor* Orb);
	//Register Orb Spwan
	void RegisterOrbSpawn(FVector Orb_Position);
	//Start Function for Multiplayer Class
	void Start();
	//Update Function for Multiplayer (Gets called in GameMode)
	void Update();		

private:
	//Checks if the message is valid and returns the message split by elements
    bool CheckMessageValidity(FString* InputMessage);
	//Creates a message base on the code provided as a parameter
	FString* CreateMessage(int OP_Code, AActor* GameObject);
	//Gets message from the queue and updates the game client
	void DeconstructMessage(FString message);		
	//Create Current Client Messages to be sent to other peers and Add them to the Outbox
	void FillOutbox();
	//Create messages to test the connection with all the peers. Runs in FillOutboxUpdate()
	void TestConnectionsMessages();
	//Creates gameplay messages and stores them in the outbox. Runs in FillOutboxUpdate()
	void GameplayMessages();
	//Creates pre game messages that are used to sync the clients. Runs in FillOutboxUpdate()
	void PreGameMessages();
	//Gets Message from the receive Queue
	FString* GetFromInbox(); 
	//Add Messages to the Send Queue
	void AddToOutbox(FString* Message); 
	
	/* Parse Messages Methods */
	//Update Game State for OP_Code = Position (4)
	void ParseCodePosition(TArray<FString> SplitMessage);
	//Update Game State for OP_Code = Orb (5)
	void ParseCodeOrb(TArray<FString> SplitMessage);
	//Update Game State for OP_Code = Position (9)
	void ParseCodeStartGame(TArray<FString> SplitMessage);
	//Parses messages 
	void ParseCodeConnectionTest(TArray<FString> SplitMessage);
	//Update Game State for OP_Code = Position (9) (CURRENTLY NOT IMPLEMENTED)
	void ParseCodeOrbSpawn(TArray<FString> SplitMessage);
	//Setter Method for m_AllPeeersReady
	void ReadyToStartTheGame(bool _AllPeeersReady);
	//GetterMEthod for m_AllPeeersReady
	bool ArePeersReady();


	//Sender Thread Pointer
	UDPThreadSender* SenderThread = nullptr;
	//Receiver Thread Pointer
	UDPThreadReceiver* ReceiverThread = nullptr;
	//Inbox Message Queue	
	TQueue<FString*, EQueueMode::Mpsc>* Inbox; 
	//Outbox Message Queue
	TQueue<FString*, EQueueMode::Mpsc>* Outbox;
	//Condition Variable (Pointer to variable in the UDPSenderThread)
	std::condition_variable* SenderConditionVariable;	
	//Pointer to the game world
	UWorld* World;
	//contains information about all the peers connected
	TArray<PeerInfo*> peersInformation;
	//Array of Actors to track state and broadcast
	TArray<AActor*> ActorsToTrack;
	//True if the peers have connected and the game has started, false otherwise
	bool m_AllPeeersReady = false;	
	//Is true if the class has run the tests on all peer connections and succeeded
	bool m_HasTestedConnections = false;
	//Instance of utility class GameSynchronization
	GameSynchronization* m_GameSynchronization = nullptr;
	
};

//Used to 
enum PeerType {
	NONE,
	BOTH
};
