// Fill out your copyright notice in the Description page of Project Settings.

#include "UDPThreadSender.h"
#include <RunnableThread.h>
#include "PlatformProcess.h"
#include "Containers/UnrealString.h"
#include "ConfigCacheIni.h"
#include "UDPThreadReceiver.h"

UDPThreadSender* UDPThreadSender::Runnable = NULL;

UDPThreadSender::UDPThreadSender(TQueue<FString*, EQueueMode::Mpsc>* _Outbox) : Outbox(_Outbox)
{	
	isFinished = false;
	hasMatchmaking = false;
	Thread = FRunnableThread::Create(this, TEXT("UDPSenderThread"), 0, TPri_BelowNormal);
	
}

UDPThreadSender::UDPThreadSender(TQueue<FString*, EQueueMode::Mpsc>* _Outbox, TArray<FString> _peersAddresses, int* _ports) : Outbox(_Outbox), peersAddresses(_peersAddresses), ports(_ports)
{
	isFinished = false;
	hasMatchmaking = true;
	Thread = FRunnableThread::Create(this, TEXT("UDPSenderThread"), 0, TPri_BelowNormal);
}

UDPThreadSender::~UDPThreadSender()
{
	UE_LOG(LogTemp, Warning, TEXT("UDP Sender Deconstruct"));		
	if (Thread)
	{
		delete Thread;
		Thread = NULL;
	}
}


UDPThreadSender * UDPThreadSender::JoyInit(TQueue<FString*, EQueueMode::Mpsc>* _Outbox)
{
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new UDPThreadSender(_Outbox);
	}
	return Runnable;
}

UDPThreadSender * UDPThreadSender::JoyInit(TQueue<FString*, EQueueMode::Mpsc>* _Outbox, TArray<FString> _peersAddresses, int* _ports)
{
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new UDPThreadSender(_Outbox, _peersAddresses, _ports);
	}
	return Runnable;
}


bool UDPThreadSender::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("UDP Sender Init"));		

	//Initialize peerAddresses array
	std::vector<std::string> tPeerAddresses = std::vector<std::string>();
	//Add peers IP's to Address Array
	if (hasMatchmaking) {
		int num_players = peersAddresses.Num();
		UE_LOG(LogTemp, Warning, TEXT("Number of addresses: %d, passed on Addresses:"), num_players);
		//Converts FString Array to Vector<string>
		for (int i = 0; i < peersAddresses.Num(); i++) {
			std::string peer = TCHAR_TO_UTF8(*peersAddresses[i]);
			tPeerAddresses.push_back(peer);
			FString toPrint = FString(peer.c_str());
			UE_LOG(LogTemp, Warning, TEXT("IP of peer: %s, port: %d"), *toPrint, ports[i]);
		}
	}
	else { //when matchmaking is turned off
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

	//Load In port number
	int32 localPort = 0; //Gets changed by ini
	GConfig->GetInt(TEXT("/Script/Engine.Network"), TEXT("localPort"), localPort, GGameIni);
	
	UDPConnection = new UDPGameplayConnection(tPeerAddresses, localPort, ports);

	//Error Logs
	char* result;
	result = UDPConnection->InitializeWinsock();
	FString fMessage(result);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *fMessage);	
	
	//Copies thesocket from the receiver connection into the sender connection
	result = UDPConnection->CopySocket(UDPThreadReceiver::Runnable->GetSocket());
	fMessage = result;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *fMessage);
	
	return true;
}

uint32 UDPThreadSender::Run()
{
	UE_LOG(LogTemp, Warning, TEXT("UDP Sender - Run"));
	//Initial wait before starting
	FPlatformProcess::Sleep(1);			

	while (StopTaskCounter.GetValue() == 0 && !isFinished)
	{	
		//UE_LOG(LogTemp, Warning, TEXT("Queque is %d"), Outbox->IsEmpty());
		//Initialize Condition Variable
		std::unique_lock<std::mutex> lock(SenderMutex); //TESTAR SE ISTO PODE ESTAR FORA DO LOOP
		//If Queue is empty wait
		while (Outbox->IsEmpty()) {
			ConditionVariable.wait(lock);
		}	
		//Convert TString to Char*			
		char* Message = TCHAR_TO_UTF8(**GetFromOutbox());
		if (Message != nullptr) {
			//Send the Message and Print the result
			FString Result = UDPConnection->Send(Message);
			//UE_LOG(LogTemp, Warning, TEXT("Message Sent %s"), *Result);
		}
		lock.unlock();
		ConditionVariable.notify_all();	
	}	
	return 0;
}

void UDPThreadSender::Stop()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop"));
	StopTaskCounter.Increment();
}

void UDPThreadSender::Shutdown()
{
	UE_LOG(LogTemp, Warning, TEXT("Shutdown"));
	if (Runnable)
	{
		Runnable->EnsureCompletion();
		delete Runnable;
		Runnable = NULL;
	}
}

std::condition_variable * UDPThreadSender::GetConditionVariable()
{
	return &ConditionVariable;
}

void UDPThreadSender::SetPeersAddresses(TArray<FString> _peersAddresses, int* _ports)
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
		UE_LOG(LogTemp, Warning, TEXT("IP of peer: %s, port: %d"), *toPrint, ports[i]);
	}
	UDPConnection->SetPeersAddresses(tPeerAddresses, ports);

}

void UDPThreadSender::EnsureCompletion()
{
	UE_LOG(LogTemp, Warning, TEXT("EnsureCompletion"));
	Stop();
	Thread->WaitForCompletion();
}

FString* UDPThreadSender::GetFromOutbox()
{
	FString* Message;
	Outbox->Dequeue(Message);
	return Message;
}

bool UDPThreadSender::IsThreadFinished()
{
	if (Runnable) return Runnable->isFinished;
	return true;
}


