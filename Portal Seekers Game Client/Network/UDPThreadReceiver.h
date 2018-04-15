// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThreadSafeCounter.h"
#include <Runnable.h>
#include "Containers/Queue.h"
#include "UDPGameplayConnection.h"

/**
* Responsible to create a thread to receive messages from other clients
* Is created by the multiplayer manager class and works as a singleton
*/
class THEPORTALSEEKERS_API UDPThreadReceiver : public FRunnable
{
public:
	//Differet constructors depending if the matchmaking is enabled
	// disabled get other peer address from the config file
	UDPThreadReceiver(TQueue<FString*, EQueueMode::Mpsc>* _Inbox);
	UDPThreadReceiver(TQueue<FString*, EQueueMode::Mpsc>* _Inbox, TArray<FString> _peersAddresses, int* _ports);
	~UDPThreadReceiver();

	static UDPThreadReceiver* Runnable;
	FRunnableThread* Thread;
	FThreadSafeCounter StopTaskCounter;
	//Function to be called to instanciate the thread, 2 declarations, depending if matchmaking is enabled
	static UDPThreadReceiver* JoyInit(TQueue<FString*, EQueueMode::Mpsc>* _Inbox);
	static UDPThreadReceiver* JoyInit(TQueue<FString*, EQueueMode::Mpsc>* _Inbox, TArray<FString> _peersAddresses, int* _ports);
	//Begin method for the thread
	virtual bool Init();
	//Thread Run Method - Contain an infinite loop
	virtual uint32 Run();
	//Stops the thread
	virtual void Stop();
	//Calls the thread shutdown
	static void Shutdown();
	//TArray<FString> to std::array<string> and passes it down to the UDPGamePlayCOnnection
	void SetPeersAddresses(TArray<FString> _peersAddresses, int* _ports);
	//Setter Method for isTesting boolean
	void SetTesting(bool _enable);
	//Getter Method of the local Address
	sockaddr_in* GetLocalAddress();
	//Getter Method for the Socket created by UDPConnection
	SOCKET* GetSocket();

private:	
	//Ensures the thread finished safely
	void EnsureCompletion();
	//Checks if the thread as finished
	static bool IsThreadFinished();
	//Adds Messages to Inbox Queue
	void AddToInbox(FString* Message);
	//Array with the adresses of all the peers
	TArray<FString> peersAddresses;
	//Array of ports of all the peers
	int* ports;
	//States if matchmaking is active
	bool hasMatchmaking;
	//Pointer to Inbox Queue. Multiplayer.cpp retreives messages from this queue
	TQueue<FString*, EQueueMode::Mpsc>* Inbox; 
	//States if the thread is finished
	bool isFinished;
	//UDP Connection Server setup 
	UDPGameplayConnection* UDPConnection = nullptr;
	//States if the connection is active
	bool* isConnected = nullptr;
	//States if IP's are being tested. If treu, ip's of receiving messages are being added to the end of received messages
	bool isTesting = false;



};
