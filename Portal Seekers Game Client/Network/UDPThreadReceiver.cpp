// Fill out your copyright notice in the Description page of Project Settings.

#include "UDPThreadReceiver.h"
#include <RunnableThread.h>
#include "PlatformProcess.h"
#include "Containers/UnrealString.h"
#include "ConfigCacheIni.h"

UDPThreadReceiver* UDPThreadReceiver::Runnable = NULL;

UDPThreadReceiver::UDPThreadReceiver(TQueue<FString*, EQueueMode::Mpsc>* _Inbox) : StopTaskCounter(0), Inbox(_Inbox)
{
	isFinished = false;	
	hasMatchmaking = false;	
	Thread = FRunnableThread::Create(this, TEXT("CommunicationThread2"), 0, TPri_BelowNormal);	
}

UDPThreadReceiver::UDPThreadReceiver(TQueue<FString*, EQueueMode::Mpsc>* _Inbox, TArray<FString> _peersAdresses, int* _ports) : StopTaskCounter(0), Inbox(_Inbox), peersAddresses(_peersAdresses), ports(_ports)
{
	isFinished = false;	
	hasMatchmaking = true;
	Thread = FRunnableThread::Create(this, TEXT("UDPThreadReceiver"), 0, TPri_BelowNormal);
}

UDPThreadReceiver::~UDPThreadReceiver()
{
	UE_LOG(LogTemp, Warning, TEXT("Deconstruct"));
	delete Thread;
	Thread = NULL;
}

UDPThreadReceiver * UDPThreadReceiver::JoyInit(TQueue<FString*, EQueueMode::Mpsc>* _Inbox)
{
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new UDPThreadReceiver(_Inbox);
	}
	return Runnable;
}

UDPThreadReceiver * UDPThreadReceiver::JoyInit(TQueue<FString*, EQueueMode::Mpsc>* _Inbox, TArray<FString> _peersAdresses, int* _ports)
{
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new UDPThreadReceiver(_Inbox, _peersAdresses, _ports);
	}
	return Runnable;
}

bool UDPThreadReceiver::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("UDP Receiver Init"));
	
	//Load In port number
	int32 localPort = 0; //Gets changed by ini
	GConfig->GetInt(TEXT("/Script/Engine.Network"), TEXT("localPort"), localPort, GGameIni);
	
	//Initialize peerAddresses array
	std::vector<std::string> tPeerAddresses = std::vector<std::string>();			
	//Add peers IP's to Address Array
	if (hasMatchmaking) {
		int num_players = peersAddresses.Num();		
		//Converts FString Array to Vector<string>
		for (int i = 0; i < peersAddresses.Num(); i++) {
			std::string peer = TCHAR_TO_UTF8(*peersAddresses[i]);
			tPeerAddresses.push_back(peer);
			//FString toPrint = FString(peer.c_str());
			//UE_LOG(LogTemp, Warning, TEXT("IP of peer: %s"), *toPrint);
		}
	}
	else { 		
		FString peerAddress(""); //Gets changed by ini
		GConfig->GetString(TEXT("/Script/Engine.Network"), TEXT("peerAddress"), peerAddress, GGameIni);
		std::string peer = TCHAR_TO_UTF8(*peerAddress);
		tPeerAddresses.push_back(peer);	

		//Load Out port Number
		int32 peerPort = 0; //Gets changed by ini
		GConfig->GetInt(TEXT("/Script/Engine.Network"), TEXT("peerPort"), peerPort, GGameIni);
		ports = new int[1];
		ports[1] = peerPort;
	}

	UDPConnection = new UDPGameplayConnection(tPeerAddresses, localPort, ports);	
	isConnected = UDPConnection->IsConnected();

	//Error Logs	
	char* result;
	result = UDPConnection->InitializeWinsock();
	FString fMessage(result);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *fMessage);

	result = UDPConnection->CreateSocket();
	fMessage = result;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *fMessage);
	
	//bind Socket
	char* iResult = UDPConnection->BindReceivingSocket();	
	fMessage = iResult;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *fMessage);
	
	return true;
}

uint32 UDPThreadReceiver::Run()
{
	UE_LOG(LogTemp, Warning, TEXT("Run Receiver"));
	//Initial wait before starting
	FPlatformProcess::Sleep(0.03);	
	while (StopTaskCounter.GetValue() == 0 && !isFinished)
	{
		if (!*isConnected) {
			UE_LOG(LogTemp, Warning, TEXT("UDP Receiver not connected"));
			return 0;
		}
		//UE_LOG(LogTemp, Warning, TEXT("Trying Receiver"));
		char* CharMessage = nullptr;
		//Call receive Message from UDPReceiver Connection Class 
		int iResult = UDPConnection->Receive(&CharMessage, isTesting);
		//After receiving -> check the result
		if (iResult > 0) {
			if (CharMessage == nullptr) {
				UE_LOG(LogTemp, Error, TEXT("Message Receive is null. Result %d"), iResult);
			}
			else {
				//Adds Message to Inbox Queue
				FString* RecvMessage = new FString(CharMessage);
				//UE_LOG(LogTemp, Warning, TEXT("Received Message: %s"), **RecvMessage)
				AddToInbox(RecvMessage);
			}
		}
		else if (iResult == 0) {
			FString RecvMessage = FString(CharMessage);
			UE_LOG(LogTemp, Warning, TEXT("Receive connection from unknowned Address. %s"), *RecvMessage);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("UDP Receive failed, result: %d, %i"), iResult, WSAGetLastError());
		}			
	}
	return 0;
}

void UDPThreadReceiver::Stop()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop"));
	StopTaskCounter.Increment();
	UDPConnection->Shutdown();	
}

void UDPThreadReceiver::EnsureCompletion()
{
	UE_LOG(LogTemp, Warning, TEXT("EnsureCompletion"));
	Stop();	
	Thread->WaitForCompletion();
}

void UDPThreadReceiver::Shutdown()
{
	UE_LOG(LogTemp, Warning, TEXT("Shutdown"));
	
	if (Runnable)
	{
		Runnable->EnsureCompletion();
		delete Runnable;
		Runnable = NULL;
	}
}

void UDPThreadReceiver::SetPeersAddresses(TArray<FString> _peersAddresses, int* _ports)
{
	peersAddresses = _peersAddresses;
	ports = _ports;
	//Initialize peerAddresses array
	std::vector<std::string> tPeerAddresses = std::vector<std::string>();
	//Converts FString Array to Vector<string>
	for (int i = 0; i < peersAddresses.Num(); i++) {
		std::string peer = TCHAR_TO_UTF8(*peersAddresses[i]);
		tPeerAddresses.push_back(peer);
		FString toPrint = FString(peer.c_str());
		//UE_LOG(LogTemp, Warning, TEXT("IP of peer: %s, port: %d"), *toPrint, ports[i]);
	}
	UDPConnection->SetPeersAddresses(tPeerAddresses, ports);

}

void UDPThreadReceiver::SetTesting(bool _enable)
{
	isTesting = _enable;
}

sockaddr_in * UDPThreadReceiver::GetLocalAddress()
{
	return UDPConnection->GetLocalAddress();
}

SOCKET * UDPThreadReceiver::GetSocket()
{
	return UDPConnection->GetSocket();
}

bool UDPThreadReceiver::IsThreadFinished()
{
	if (Runnable) return Runnable->isFinished;
	return true;
}

void UDPThreadReceiver::AddToInbox(FString* Message)
{
	//UE_LOG(LogTemp, Warning, TEXT("Add message to queue: %s"), **Message);
	Inbox->Enqueue(Message);
}

