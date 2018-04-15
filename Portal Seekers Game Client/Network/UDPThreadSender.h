// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThreadSafeCounter.h"
#include <Runnable.h>
#include "UDPGameplayConnection.h"
#include "Containers/Queue.h"
#include <condition_variable>
#include <mutex>

/**
* Responsible to create a thread to send messages to other clients
* Is created by the multiplayer manager class and works as a singleton
*/
class THEPORTALSEEKERS_API UDPThreadSender : public FRunnable
{
public:
	//Different constructors depending if the matchmaking is enabled
	// disabled get other peer address from the config file
	UDPThreadSender(TQueue<FString*, EQueueMode::Mpsc>* _SendQueue);
	UDPThreadSender(TQueue<FString*, EQueueMode::Mpsc>* _Outbox, TArray<FString> _peersAddresses, int* _ports);
	~UDPThreadSender();
	//Singleton reference
	static UDPThreadSender* Runnable;
	//Reference to the thread instance
	FRunnableThread* Thread;
	//Thread Safe Counter, necessary to the Unreal Thread Logic
	FThreadSafeCounter StopTaskCounter;
	//Function to be called to instanciate the thread, if matchmaking is disabled
	static UDPThreadSender* JoyInit(TQueue<FString*, EQueueMode::Mpsc>* _SendQueue);
	//Instanciates the Thread is the matchmaking is enabled
	static UDPThreadSender* JoyInit(TQueue<FString*, EQueueMode::Mpsc>* _SendQueue, TArray<FString> _peersAddresses, int* _ports);
	//Begin method for the thread
	virtual bool Init();
	//Thread Run Method - Contain an infinite loop
	virtual uint32 Run();
	//Stops the thread
	virtual void Stop();
	//Calls the thread shutdown
	static void Shutdown();
	//Getter for the condition variable
	std::condition_variable* GetConditionVariable();
	//TArray<FString> to std::array<string> and passes it down to the UDPGamePlayCOnnection
	void SetPeersAddresses(TArray<FString> _peersAddresses, int* _ports);

private:
	//Ensures the thread finished safely
	void EnsureCompletion();
	//Checks if the thread as finished
	static bool IsThreadFinished();
	//Gets Message from the receive Queue
	FString* GetFromOutbox(); 
	//Queue Containing Messages to be sent through the connection (multiplayer.cpp inserted the messages)
	TQueue<FString*, EQueueMode::Mpsc>* Outbox; 	
	//Array with the adresses of all the peers
	TArray<FString> peersAddresses;	
	//Ports of all the peers
	int* ports;
	//States if matchmaking is active
	bool hasMatchmaking;
	//Mutex and ConditionVariable used to handle consumer producer problem concerning the outbox queue
	std::mutex SenderMutex;
	std::condition_variable ConditionVariable;
	//states if the thread is finish
	bool isFinished;
	//Reference to UDPConnection class
	UDPGameplayConnection* UDPConnection = nullptr; 
};


