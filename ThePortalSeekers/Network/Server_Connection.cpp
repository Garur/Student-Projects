// Fill out your copyright notice in the Description page of Project Settings.

#include "Server_Connection.h"
#include "ConfigCacheIni.h"
#include "TCPReceiverThread.h"
#include "Engine/World.h"
#include "ThePortalSeekersGameMode.h"

Server_Connection::Server_Connection(UWorld* _World)
{
	Inbox = new TQueue<FString*, EQueueMode::Mpsc>();	
	World = _World;

	FString serverAddress("");//Get server address from game.ini file
	GConfig->GetString(TEXT("/Script/Engine.Network"), TEXT("serverAddress"), serverAddress, GGameIni);
	int serverPort; //Get server port from game.ini file
	GConfig->GetInt(TEXT("/Script/Engine.Network"), TEXT("serverPort"), serverPort, GGameIni);	
	int localPort; //Get local port from game.ini file
	GConfig->GetInt(TEXT("/Script/Engine.Network"), TEXT("localPort"), localPort, GGameIni);
	//Instanciate the server connection 
	TCPConnection = new TCPServerConnection(TCHAR_TO_UTF8(*serverAddress), serverPort, localPort);
		
	char* result;//Used for error reports
	//Initialize Winsock
	result = TCPConnection->InitializeWinsock();
	FString fMessage(result);//Used for error reports
	UE_LOG(LogTemp, Warning, TEXT("%s"), *fMessage);	
	
	//Create TCP Socket
	result = TCPConnection->CreateSocket();
	fMessage = result;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *fMessage);
	//Bind Local Port
	int iResult = TCPConnection->BindLocalPort();
	if (iResult != 0)
		UE_LOG(LogTemp, Warning, TEXT("Error binding socket: %d"), iResult);	
	//Connect to the server
	result = TCPConnection->Connect();
	fMessage = result;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *fMessage);
	//Send Initial play message to the server
	SendMessageToServer(GameMessageType::OP_PLAY);

	//Start receiving messages thread
	SOCKET* conn_socket = TCPConnection->GetSocket();
	TCPReceiverThread::Shutdown();//resets the connection
	bool* isConnected = TCPConnection->IsConnected();
	TCPReceiverThread* Receiver = TCPReceiverThread::JoyInit(Inbox, conn_socket, isConnected);
}

Server_Connection::~Server_Connection()
{
	UE_LOG(LogTemp, Warning, TEXT("Matchmaking Deconstructor"));
	//TCPReceiverThread::Shutdown();
	//delete TCPConnection
}

void Server_Connection::Tick()
{
	//Retreive messages from inbox and updates the game, maximum 25 per tick
	while (!Inbox->IsEmpty()) {
		//Get Message from inbox			
		FString* InputMessage = GetFromInbox();
		if (InputMessage == nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("Message from inbox is null"));
			return;
		}
		TArray<FString> SplitMessage;
		InputMessage->ParseIntoArray(SplitMessage, TEXT(":"), false);

		//UE_LOG(LogTemp, Warning, TEXT("Update: %s"), **InputMessage);
		//Deconstruct the message
		DeconstructMessage(SplitMessage);
	}
}

TArray<PeerInfo*> Server_Connection::GetPeersInformation()
{
	return PeersInformation;
}

int Server_Connection::GetNumberPlayer()
{
	return NumPlayers;
}

int Server_Connection::GetPlayerID()
{
	return PlayerID;
}

bool Server_Connection::IsMatchmakingComplete()
{
	return m_IsMatchmakingComplete;
}

void Server_Connection::SendMessageToServer(int op_code)
{
	//Create message based on the op_code
	FString message = CreateMessage(op_code);
	char* char_msg(TCHAR_TO_UTF8(*message));
	UE_LOG(LogTemp, Warning, TEXT("Char Message Created: %s"), *message);
	//Send message
	int result = TCPConnection->Send(char_msg);	
	UE_LOG(LogTemp, Warning, TEXT("Bytes Sent to server: %d"), result);
}

FString* Server_Connection::GetFromInbox()
{
	FString* Message = nullptr;
	Inbox->Dequeue(Message);
	return Message;
}
FString Server_Connection::CreateMessage(int OP_Code)
{
	switch (OP_Code)
	{
	case OP_PLAY:
	{
		//Get List of Local IP's
		FString str_address = TCPConnection->GetLocalAddresses();
		int port = TCPConnection->GetLocalPort();
		FString message = FString::Printf(TEXT("%d:%s:%d"), OP_Code, *str_address, port);		
		return message;
		break;
	}
	case OP_FINISH:
	{
		AThePortalSeekersGameMode* GameMode = (AThePortalSeekersGameMode*) World->GetAuthGameMode();
		int winner = GameMode->GetWinner();
		FString time_started = GameMode->GetTimeStarted();
		FDateTime date = FDateTime();
		FString time_finished = date.GetDate().ToString();
		FString message = FString::Printf(TEXT("%d:%d:%s:%s;"), OP_Code, winner, *time_started, *time_finished);
		return message;
		break;
	}
	default:
		break;
	}
	UE_LOG(LogTemp, Warning, TEXT("Failed to create Message"));
	return FString();

}

void Server_Connection::DeconstructMessage(TArray<FString> message)
{
	int32 OP_Code = FCString::Atoi(*message[0]);
	switch (OP_Code)
	{
		case OP_PLAY:
		{
			break;
		}
		case OP_ID:
		{
			int tempID = FCString::Atoi(*message[1]);
			if (tempID > 4 || tempID < 0) {
				UE_LOG(LogTemp, Error, TEXT("Received peer ID invalid: %d"), tempID);
			}
			else {
				PlayerID = tempID;
			}

			int tempNum = FCString::Atoi(*message[2]);
			if (tempNum > 5 || tempNum < 2) {
				UE_LOG(LogTemp, Error, TEXT("Received number of players invalid: %d"), tempNum);
			}
			else {
				NumPlayers = tempNum;
			}
			break;
		}
		case OP_IP:
		{
			if (NumPlayers > 5 || NumPlayers < 2) {
				UE_LOG(LogTemp, Error, TEXT("Number of players is invalid"));
				return;
			}

			bool peerFound = false;
			if (PeersInformation.Num() > 0) {
				//Get ID
				int peerID = FCString::Atoi(*message[3]);
				//Loop through received address informatin struct 
				for (int i = 0; i < PeersInformation.Num(); i++) {
					//If ID already registered 
					if (peerID == PeersInformation[i]->id) {
						//Save the address for that player id
						PeersInformation[i]->addresses.Add(message[1]);
						PeersInformation[i]->ports.Add(FCString::Atoi(*message[2]));
						peerFound = true;
					}					
				}
			}
			
			if(!peerFound) {
				//Instanciate a new struct to save information about peer
				PeerInfo* p_info = new PeerInfo();
				//Save the outer network address
				p_info->addresses.Add(message[1]);
				//Save the port number of the peer
				p_info->ports.Add(FCString::Atoi(*message[2]));
				//Save the game id of the peer
				p_info->id = FCString::Atoi(*message[3]);
				//Add the peerInfo to the Array
				PeersInformation.Add(p_info);
				UE_LOG(LogTemp, Warning, TEXT("Peer info added. ID %d"), p_info->id);
			}
				
			UE_LOG(LogTemp, Warning, TEXT("Received peer IP: %s and port %s"), *message[1], *message[2]);
				
			//Checks if the matchmaking is complete
			if (PeersInformation.Num() == NumPlayers - 1 && !m_IsMatchmakingComplete) {
				m_IsMatchmakingComplete = true;
				UE_LOG(LogTemp, Warning, TEXT("Matchmaking is Complete"));
			}

			break;
		}
		case OP_INFO:
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("%s"), *message[1]));
			UE_LOG(LogTemp, Warning, TEXT("Broadcast Received: %s"), *message[1]);
			break;
		}
		default:
			break;
	}
}
