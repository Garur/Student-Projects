// Fill out your copyright notice in the Description page of Project Settings.

#include "Multiplayer.h"
#include "ConfigCacheIni.h"
#include "UObject/UObjectIterator.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "MyPlayerController.h"
#include "Seeker.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ThePortalSeekersGameMode.h"

Multiplayer::Multiplayer(UWorld* _World) : World(_World)
{
	Inbox = new TQueue<FString*, EQueueMode::Mpsc>();
	Outbox = new TQueue<FString*, EQueueMode::Mpsc>();
	ActorsToTrack = TArray<AActor*>();
	
	//Gets the multiplayer value from the config file and enables the multiplayer accordingly 
	int32 hasMultiplayer;
	GConfig->GetInt(TEXT("/Script/Engine.Network"), TEXT("multiplayer"), hasMultiplayer, GGameIni);

	//Checks if matchmaking is active
	bool hasMatchmaking;
	GConfig->GetBool(TEXT("/Script/Engine.Network"), TEXT("matchmaking"), hasMatchmaking, GGameIni);
	if (!hasMatchmaking) {
		m_HasTestedConnections = true;
	}

	//If multiplayer is on -> Starts the Threads
	if (hasMultiplayer == BOTH) {
		UDPThreadReceiver::Shutdown();
		ReceiverThread = UDPThreadReceiver::JoyInit(Inbox);
		UDPThreadSender::Shutdown();
		SenderThread = UDPThreadSender::JoyInit(Outbox);
		//Get sender condition variable, so the multiplayer manager can notify the sender thread 
		SenderConditionVariable = SenderThread->GetConditionVariable();
	}

	//Gets Game Player Controller
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(World, 0));
	if (PlayerController != nullptr) {
		//Get local Player ID
		int id = *PlayerController->GetCurrentPlayerID();
		//Initializes class that will take care of the game clocks
		m_GameSynchronization = new GameSynchronization(World, id, peersInformation.Num());
	}
	
	
}

Multiplayer::Multiplayer(UWorld* _World, TArray<PeerInfo*> _peersInfo) : World(_World), peersInformation(_peersInfo)
{
	Inbox = new TQueue<FString*, EQueueMode::Mpsc>();
	Outbox = new TQueue<FString*, EQueueMode::Mpsc>();	
	ActorsToTrack = TArray<AActor*>();

	//Sets the peer type based on the config file (Server, Client, Both or None)
	int32 hasMultiplayer; //Changed in game ini file;
	GConfig->GetInt(TEXT("/Script/Engine.Network"), TEXT("multiplayer"), hasMultiplayer, GGameIni);

	//Checks if matchmaking is active
	bool hasMatchmaking; 
	GConfig->GetBool(TEXT("/Script/Engine.Network"), TEXT("matchmaking"), hasMatchmaking, GGameIni);
	if (!hasMatchmaking) {
		m_HasTestedConnections = true;
	}

	//Get All possible Addresses and ports of the peers to first test the connections
	TArray<FString> tPeersAddresses = TArray<FString>();	
	TArray<int> tPorts = TArray<int>();
	UE_LOG(LogTemp, Warning, TEXT("PeersInformation Num: %d"), peersInformation.Num());
	for (int i = 0; i < peersInformation.Num(); i++) {
		UE_LOG(LogTemp, Warning, TEXT("Address num: %d"), peersInformation[i]->addresses.Num());
		for (int j = 0; j < peersInformation[i]->addresses.Num(); j++) {
			tPeersAddresses.Add(peersInformation[i]->addresses[j]);
			tPorts.Add(peersInformation[i]->ports[j]);
		}		
	}

	//Copies port numbers from TArray to int* to be passed to the connection layer
	int* tempPorts = new int[tPorts.Num()];	
	UE_LOG(LogTemp, Warning, TEXT("Num of Addresses passed to the Receiver and Sender Threads: %d"), tPeersAddresses.Num());
	for (int i = 0; i < tPorts.Num(); i++) {
		UE_LOG(LogTemp, Warning, TEXT("Port %d: %d"), i, tPorts[i]);
		tempPorts[i] = tPorts[i];
	}

	//If multiplayer is on -> Starts the Threads
	if (hasMultiplayer == BOTH) {
		UDPThreadReceiver::Shutdown();
		//Initialize with all the possible IP addresses to test the connections
		ReceiverThread = UDPThreadReceiver::JoyInit(Inbox, tPeersAddresses, tempPorts);
		UDPThreadSender::Shutdown();
		SenderThread = UDPThreadSender::JoyInit(Outbox, tPeersAddresses, tempPorts);
		//Get sender condition variable, so the multiplayer manager can notify the sender thread 
		SenderConditionVariable = SenderThread->GetConditionVariable();
	}

	//Gets Game Player Controller
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(World, 0));
	if (PlayerController != nullptr) {
		//Get local Player ID
		int id = *PlayerController->GetCurrentPlayerID();
		//Initializes class that will take care of the game clocks
		m_GameSynchronization = new GameSynchronization(World, id, peersInformation.Num());
	}
}

Multiplayer::~Multiplayer()
{
	UDPThreadReceiver::Shutdown();
	//UDPThreadSender::Shutdown();
}

void Multiplayer::RegisterActor(AActor * ActorToRegister)
{	
	ActorsToTrack.Add(ActorToRegister);
	//UE_LOG(LogTemp, Warning, TEXT("Actors to Track: %d"), ActorsToTrack.Num());
}

void Multiplayer::UnregisterActor(AActor * ActorToUnregister)
{
	for (int i = 0; i < ActorsToTrack.Num(); i++) {
		if (ActorsToTrack[i] == ActorToUnregister) {
			ActorsToTrack.Remove(ActorToUnregister);
			return;
		}
	}
}

void Multiplayer::Start()
{
	TestConnectionsMessages();
}

void Multiplayer::Update()
{
	//Update game sync tick
	m_GameSynchronization->Tick();
	//Create Messages to send messages and places them in the outbox
	FillOutbox();
	//Retreive messages from inbox and updates the game, maximum 40 per tick
	int counter = 0;
	while (!Inbox->IsEmpty() || counter <40) {
		//Get Message from inbox	
		FString* InputMessage = GetFromInbox();
		if (InputMessage == nullptr) {
			return;
		}		
		bool isValid =  CheckMessageValidity(InputMessage);
		if (isValid) {
			//UE_LOG(LogTemp, Warning, TEXT("Multiplayer Update - Message Received: %s"), **InputMessage);
			//Deconstruct the message and update the game
			DeconstructMessage(*InputMessage);
		}
		else {
			UE_LOG(LogTemp, Log, TEXT("Invalid message Received"));
		}
		counter++;
	}	
}

bool Multiplayer::CheckMessageValidity(FString* InputMessage) {
	
	if (InputMessage == nullptr) {
		return false;
	}
	TArray<FString> _SplitMessage = TArray<FString>();
	//splits message and inserts it into an array
	int numElements = InputMessage->ParseIntoArray(_SplitMessage, TEXT(":"), false); 
	//Discards Messages with the number of arguments less or equal to 1
	if (numElements <= 1) {
		return false;
	}
	//Checks the message max number of arguments
	if (numElements > MAX_NUM_MSG_ARG) {
		return false;
	}
	return true;
}

void Multiplayer::FillOutbox() {
	//Game has started set output messages 
	if (!m_HasTestedConnections) {		
		int counter = 0;
		AThePortalSeekersGameMode* gameMode = (AThePortalSeekersGameMode*)World->GetAuthGameMode();
		for (int i = 0; i < peersInformation.Num(); i++) {
			if (peersInformation[i]->chosenAddress.Compare("\0") != 0) {
				//Player Connection has been confirmed -> increase counter
				counter++;	
				//When all the players have been confirmed a connection 
				if (counter == gameMode->GetNumberOfPlayers() - 1) {					
					UE_LOG(LogTemp, Warning, TEXT("Connections Successfully tested"));
					//All Connections have been tested
					m_HasTestedConnections = true;
					//Disables testing connection mode in sender thread
					ReceiverThread->SetTesting(false);					
				}
			}
		}
		if (m_HasTestedConnections) {
			//Copy choosen address and ports
			TArray<FString> tAddresses = TArray<FString>();
			int* tPorts = new int[peersInformation.Num()];
			for (int i = 0; i < peersInformation.Num(); i++) {
				tAddresses.Add(peersInformation[i]->chosenAddress);
				tPorts[i] = peersInformation[i]->choosenPort;
			}
			//Sets new Addresss and ports to validate the messages received in the Receiver Thread 				
			ReceiverThread->SetPeersAddresses(tAddresses, tPorts);
			//Sets new Addresss and ports to validate the messages received in th Sender Thread 				
			SenderThread->SetPeersAddresses(tAddresses, tPorts);
		}

	}else if (ArePeersReady()) {		
		GameplayMessages();
	}
	else {		
		PreGameMessages();
	}	
}

//Creates test connection messages
void Multiplayer::TestConnectionsMessages()
{	
	//Sets the receiver to testing connections mode
	ReceiverThread->SetTesting(true);	
	//Get operation code
	int op_code = Multiplayer::OP_TEST;	
	AThePortalSeekersGameMode* gameMode = (AThePortalSeekersGameMode*)World->GetAuthGameMode();
	//Get the player ID
	int playerID = gameMode->GetPlayerID();
	//Loop through every peer
	for (int i = 0; i < peersInformation.Num(); i++) {
		//Check if peer does not has a confirmed address to comunicate with
		if (peersInformation[i]->chosenAddress.Compare("\0") == 0) {
			//Create Messages with the id of this client and client to test the comunication		
			int destID = peersInformation[i]->id;
			//Message FORMAT -> OP_CODE:DESTINATION_ID:ORIGIN_ID
			FString* message = new FString(FString::Printf(TEXT("%d:%d:%d"), op_code, destID, playerID));
			// Add message to outbox
			AddToOutbox(message);
			//Print Confirmation
			UE_LOG(LogTemp, Log, TEXT("Sent Test Con Message: %s"), **message);			
		}
	}		
}

void Multiplayer::GameplayMessages()
{
	//check if there are actors registered to send updates	
	if (ActorsToTrack.Num() <= 0) {
		return;
	}
	// Loop through all the Actor being tracked
	for (int i = 0; i < ActorsToTrack.Num(); i++) {
		int OP_Code = Multiplayer::OP_POSITION;
		//Create Position Message
		FString* OutputMessage = CreateMessage(OP_Code, ActorsToTrack[i]);
		//UE_LOG(LogTemp, Warning, TEXT("OutPut Message: %s"), **OutputMessage);
		//Add Message to Outbox
		AddToOutbox(OutputMessage);
	}
}

void Multiplayer::PreGameMessages()
{
	int OP_Code = Multiplayer::OP_START_GAME;
	//Create a start game Message
	FString* OutputMessage = CreateMessage(OP_Code, ActorsToTrack[0]);
	//UE_LOG(LogTemp, Warning, TEXT("Added message to in box %s"), **OutputMessage);
	//Add Message to Outbox
	AddToOutbox(OutputMessage);
}

void Multiplayer::RegisterOrbPickup(bool isPickup, AActor* Player, AActor* Orb)
{
	//Get the Operation Code
	int OP_Code = Multiplayer::OP_ORB;
	AThePortalSeekersGameMode* gameMode = (AThePortalSeekersGameMode*)World->GetAuthGameMode();
	int playerID = gameMode->GetPlayerID();
	int vectorClock = m_GameSynchronization->GetLocalVectorClock();
	FString timeStamp = m_GameSynchronization->GetCurrentClientTime().ToString();
	FString PlayerLabel = Player->GetName();
	FString OrbLabel = Orb->GetName();
	//Creates message relative to a orb pickup
	FString* OutputMessage = new FString(FString::Printf(TEXT("%d:%d:%d:%s:%d:%s:%s"), 
		OP_Code, 
		playerID,
		vectorClock,
		*timeStamp,
		isPickup,
		*PlayerLabel,
		*OrbLabel
		));
	AddToOutbox(OutputMessage);
}

void Multiplayer::RegisterOrbSpawn(FVector Orb_Position) {
	//Get the Operation Code
	int OP_Code = Multiplayer::OP_ORB_SPAWN;
	AThePortalSeekersGameMode* gameMode = (AThePortalSeekersGameMode*)World->GetAuthGameMode();
	int playerID = gameMode->GetPlayerID();
	int vectorClock = m_GameSynchronization->GetLocalVectorClock();
	FString timeStamp = m_GameSynchronization->GetCurrentClientTime().ToString();	
	//Creates message relative to a orb spawn containing the orb position
	FString* OutputMessage = new FString(FString::Printf(TEXT("%d:%d:%d:%s:%s:%s:%s"),
		OP_Code,
		playerID,
		vectorClock,
		*timeStamp,
		*FString::SanitizeFloat(Orb_Position.X),
		*FString::SanitizeFloat(Orb_Position.Y),
		*FString::SanitizeFloat(Orb_Position.Z)
	));
	AddToOutbox(OutputMessage);
}


void Multiplayer::AddToOutbox(FString* Message)
{
	//UE_LOG(LogTemp, Warning, TEXT("AddToOutbox: %s"), **Message);
	//Put message in the outbox queue
	Outbox->Enqueue(Message);
	//Notify Sender Thread that it can resume
	SenderConditionVariable->notify_all();
}

FString * Multiplayer::CreateMessage(int OP_Code, AActor* GameObject)
{
	switch (OP_Code)
	{
		case OP_DIE:

			break;	
		case OP_RES:
	
			break;
		case OP_RES_START:

			break;
		case OP_RES_FINISH:

			break;
		case OP_POSITION:
		{			
			AThePortalSeekersGameMode* gameMode = (AThePortalSeekersGameMode*)World->GetAuthGameMode();
			int playerID = gameMode->GetPlayerID();
			int vectorClock = m_GameSynchronization->GetLocalVectorClock();
			FString timeStamp = m_GameSynchronization->GetCurrentClientTime().ToString();
			FVector Position = GameObject->GetActorTransform().GetLocation();	
			//UE_LOG(LogTemp, Warning, TEXT("Player Position: %s"), *Position.ToString());
			FRotator Rotation = GameObject->GetActorRotation();
			//Object Identifier
			FString Label = GameObject->GetName();
		
			return new FString(FString::Printf(TEXT("%d:%d:%d:%s:%s:%s:%s:%s:%s:%s:%s"),
				OP_Code,
				playerID,
				vectorClock,
				*timeStamp,
				*Label,
				*FString::SanitizeFloat(Position.X), 
				*FString::SanitizeFloat(Position.Y),
				*FString::SanitizeFloat(Position.Z),
				*FString::SanitizeFloat(Rotation.Roll),
				*FString::SanitizeFloat(Rotation.Pitch),
				*FString::SanitizeFloat(Rotation.Yaw)));
			break;
		}		
		case OP_PILLAR_START:

			break;
		case OP_PILLAR_FINISH:

			break;
		case OP_PORTAL:

			break;
		case OP_START_GAME:	
		{
			AThePortalSeekersGameMode* gameMode = (AThePortalSeekersGameMode*)World->GetAuthGameMode();
			int playerID = gameMode->GetPlayerID();
			return new FString(FString::Printf(TEXT("%d:%d:ready"), OP_Code, playerID));
			break;
		}
		default:
			break;
	}
	UE_LOG(LogTemp, Warning, TEXT("Failed to create Message"));
	return NULL;
	
}

FString* Multiplayer::GetFromInbox()
{
	FString* Message = nullptr;
	Inbox->Dequeue(Message);
	return Message;
}


void Multiplayer::DeconstructMessage(FString Message)
{			
	TArray<FString> SplitMessage;
	Message.ParseIntoArray(SplitMessage, TEXT(":"), false);
	int32 OP_Code = FCString::Atoi(*SplitMessage[0]);

	switch (OP_Code)
	{
	case OP_NULL: {
		UE_LOG(LogTemp, Warning, TEXT("Code: Someone try to send a null message. Nice Try!"));
		break;
	}
	case OP_DIE:
		UE_LOG(LogTemp, Warning, TEXT("Code: DIE"));
		break;
	case OP_RES:
		UE_LOG(LogTemp, Warning, TEXT("Code: RES"));
		break;
	case OP_RES_START:
		UE_LOG(LogTemp, Warning, TEXT("Code: RES_START"));
		break;
	case OP_RES_FINISH:
		UE_LOG(LogTemp, Warning, TEXT("Code: RES_FINISH"));
		break;
	case OP_POSITION:
	{			
		ParseCodePosition(SplitMessage);
		break;
	}
	case OP_ORB:	
	{	
		ParseCodeOrb(SplitMessage);
		break;
	}		
	case OP_PILLAR_START:
		UE_LOG(LogTemp, Warning, TEXT("Code: PILLAR_START"));
		break;
	case OP_PILLAR_FINISH:
		UE_LOG(LogTemp, Warning, TEXT("Code: PILLAR_FINISH"));
		break;
	case OP_PORTAL:
		UE_LOG(LogTemp, Warning, TEXT("Code: PORTAL"));
		break;
	case OP_ORB_SPAWN:
	{
		UE_LOG(LogTemp, Warning, TEXT("Code: ORB SPAWN"));
		ParseCodeOrbSpawn(SplitMessage);
		break;
	}		
	case OP_START_GAME:
	{
		ParseCodeStartGame(SplitMessage);
		break;
	}
	case OP_TEST:
	{
		ParseCodeConnectionTest(SplitMessage);	
		break;
	}
	default:
		break;
	}	
	return;
}

void Multiplayer::ParseCodePosition(TArray<FString> SplitMessage)
{	
	ReadyToStartTheGame(true);
	for (TObjectIterator<AThePortalSeekersCharacter> Itr; Itr; ++Itr)
	{		
		if (Itr->GetWorld() != World) {
			//UE_LOG(LogTemp, Error, TEXT("Different World"))
			continue;
		}
		
		//Compare the name of the character
		if (Itr->GetName().Compare(SplitMessage[4]) == 0) {
			
			int id = FCString::Atoi(*SplitMessage[1]);
			int clock_value = FCString::Atoi(*SplitMessage[2]);
			if (!m_GameSynchronization->IncrementVectorClockValue(id, clock_value)) {
				return;
			}
				
			//Create new rotation and apply it
			FRotator NewRotation = FRotator(FCString::Atof(*SplitMessage[9]), FCString::Atof(*SplitMessage[10]), FCString::Atof(*SplitMessage[8]));
			//Create new position vector and apply it
			FVector NewPosition = FVector(FCString::Atof(*SplitMessage[5]), FCString::Atof(*SplitMessage[6]), FCString::Atof(*SplitMessage[7]));


			//m_GameSynchronization->CheckTimeDifference(SplitMessage[2]);

			AAIController* myController = Itr->GetAIController();
			if (myController != nullptr) {
				myController->MoveToLocation(NewPosition, 0.1f, true, false, false, true, 0, true);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Controller not found"));
			}
			//Break as soon has the actor as been found and updated
			break;
		}		
	}		
}

void Multiplayer::ParseCodeOrb(TArray<FString> SplitMessage)
{
	int index = 0; //Specifies if the player and the orb has been found
	AActor* Orb = nullptr;
	ASeeker* Seeker = nullptr;
	//Iterate through the level actors
	for (TObjectIterator<AActor> Itr; Itr; ++Itr)
	{
		int id = FCString::Atoi(*SplitMessage[1]);
		int clock_value = FCString::Atoi(*SplitMessage[2]);		
		if (Itr->GetWorld() != World) {
			UE_LOG(LogTemp, Error, TEXT("Different World"))
				continue;
		}
		//Find Player
		if (Itr->GetName().Compare(SplitMessage[5]) == 0) {
			Seeker = Cast<ASeeker>(*Itr);
			index++;
		}
		//Find Orb
		if (Itr->GetName().Compare(SplitMessage[6]) == 0) {
			Orb = *Itr;
			index++;			
		}
		//When both have been found
		if (index == 2) {					
			//If the orb was pickup
			if (FCString::Atoi(*SplitMessage[4]) == 1)
			{				
				Seeker->Pick(Orb);
			}
			else { // Else orb was dropped				
				Seeker->Drop();
			}
			break;
		}
	}
}


void Multiplayer::ParseCodeStartGame(TArray<FString> SplitMessage)
{
	if (!m_HasTestedConnections) {
		return;
	}
	int id = FCString::Atoi(*SplitMessage[1]);
	int counter = 0;
	//Loops through all the peers
	for (int i = 0; i < peersInformation.Num(); i++) {
		//If Id received in the message matchs the id of peer
		if (peersInformation[i]->id == id){ 
			//Set this peer as ready
			if (!peersInformation[i]->ready) {
				peersInformation[i]->ready = true;
			}			
		}
		//Increaments counter for each peer ready
		if (peersInformation[i]->ready) {
			counter++;
		}		
	}
	//If all the peers are ready -> game can start
	if (counter == peersInformation.Num()) {
		ReadyToStartTheGame(true);
		Outbox->Empty();
		//if all peers are ready and there is still messages being received
		//PreGameMessages();
		UE_LOG(LogTemp, Warning, TEXT("All peers are ready to play"));
	}	
	
}
// Parses Messages with connection testing OP_CODE
void Multiplayer::ParseCodeConnectionTest(TArray<FString> SplitMessage)
{	
	AThePortalSeekersGameMode* gameMode = (AThePortalSeekersGameMode*)World->GetAuthGameMode();
	//Check if world is valid
	if (gameMode == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Multiplayer Parse Connection Test: Game Mode is null"));
		return;
	}
	//Check if the message received has an IP address attached by the communication layer
	if (SplitMessage.Num() < 4) {
		UE_LOG(LogTemp, Error, TEXT("Multiplayer Parse Connection Test: - Message Received without IP"));
		return;
	}
	//Get Player ID to compare with the id received in the message
	int playerID = gameMode->GetPlayerID();
	//If the id on the origin is the same to the id of this player -> the message was replicated and received back
	if (FCString::Atoi(*SplitMessage[2]) == playerID) {
		//If connections have all been tested, no need to run the rest of the algorythm
		if (!m_HasTestedConnections) {			
			//Loop through all the peers information struct to see who has send it
			for (int i = 0; i < peersInformation.Num(); i++) {
				//The destination id, here represents the peer that answered
				if (FCString::Atoi(*SplitMessage[1]) == peersInformation[i]->id) {
					//Check if this peer does not have a tested address
					if (peersInformation[i]->chosenAddress.Compare("\0") == 0) {
						FString address = *SplitMessage[3];
						for (int j = 0; j < peersInformation[i]->addresses.Num(); j++) {
							//Check what address was received
							if (peersInformation[i]->addresses[j].Compare(address) == 0) {
								//Set the chosen ip address to the player with this id
								peersInformation[i]->chosenAddress = peersInformation[i]->addresses[j];
								peersInformation[i]->choosenPort = peersInformation[i]->ports[j];
								UE_LOG(LogTemp, Warning, TEXT("Chosen Address set: %s, port: %d"), *peersInformation[i]->chosenAddress, peersInformation[i]->choosenPort);
							}							
						}
					}
				}
			}
		}		
	}
	//If message was destinated for this peer
	else if (FCString::Atoi(*SplitMessage[1]) == playerID) {		
		//Replicate and resend the message and send it
		FString* message = new FString(FString::Printf(TEXT("%s:%s:%s"), *SplitMessage[0], *SplitMessage[1], *SplitMessage[2]));		
		AddToOutbox(message);		
		//If connections have all been tested, no need to run the rest of the algorythm
		if (!m_HasTestedConnections) {
			//Loop through all the peers information struct to see who has send it
			for (int i = 0; i < peersInformation.Num(); i++) {
				//The destination id, here represents the peer that answered
				if (FCString::Atoi(*SplitMessage[2]) == peersInformation[i]->id) {
					//Check if this peer does not have a tested address
					if (peersInformation[i]->chosenAddress.Compare("\0") == 0) {
						FString address = *SplitMessage[3];
						for (int j = 0; j < peersInformation[i]->addresses.Num(); j++) {
							//Check what address was received
							if (peersInformation[i]->addresses[j].Compare(address) == 0) {
								//Set the chosen ip address to the player with this id
								peersInformation[i]->chosenAddress = peersInformation[i]->addresses[j];
								peersInformation[i]->choosenPort = peersInformation[i]->ports[j];
								UE_LOG(LogTemp, Warning, TEXT("Chosen Address set: %s, port: %d"), *peersInformation[i]->chosenAddress, peersInformation[i]->choosenPort);
							}
						}
					}
				}
			}
		}
	}//else message is discarded
	
}

void Multiplayer::ParseCodeOrbSpawn(TArray<FString> SplitMessage)
{
	//Create new position vector and apply it
	FVector NewPosition = FVector(FCString::Atof(*SplitMessage[4]), FCString::Atof(*SplitMessage[5]), FCString::Atof(*SplitMessage[6]));
}

void Multiplayer::ReadyToStartTheGame(bool _AllPeeersReady)
{
	if (m_AllPeeersReady != _AllPeeersReady) {
		UE_LOG(LogTemp, Warning, TEXT("All peers are ready to start the game"));
		m_AllPeeersReady = _AllPeeersReady;
	}	
}

bool Multiplayer::ArePeersReady()
{
	return m_AllPeeersReady;
}







