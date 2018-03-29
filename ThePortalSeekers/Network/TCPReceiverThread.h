// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThreadSafeCounter.h"
#include <Runnable.h>
#include "Containers/Queue.h"
#include<stdio.h>
#include <winsock2.h>
#include <Ws2tcpip.h> 

#define DEFAULT_BUFLEN 512

/**
 * Responsible to create a thread to receive messages from the server
 * Is created by the server connection class
 */
class THEPORTALSEEKERS_API TCPReceiverThread : public FRunnable
{
public:	
	TCPReceiverThread(TQueue<FString*, EQueueMode::Mpsc>* _Inbox, SOCKET* _socket, bool* isConnected);
	~TCPReceiverThread();
	//Methods and variables to make the thread a singleton 
	static TCPReceiverThread* Runnable;
	FRunnableThread* Thread;
	FThreadSafeCounter StopTaskCounter;	
	static TCPReceiverThread* JoyInit(TQueue<FString*, EQueueMode::Mpsc>* _Inbox, SOCKET* _socket, bool* isConnected);
	//Begin method for the thread
	virtual bool Init();
	//Thread Run Method - Contain an infinite loop
	virtual uint32 Run();
	//Stops the thread
	virtual void Stop();
	//Calls the thread shutdown
	static void Shutdown();

private:
	//Ensures the thread finished safely
	void EnsureCompletion();
	//Checks if the thread as finished
	static bool IsThreadFinished();
	//Receive method
	int Receive(char **OutMessage);
	//Adds Messages to Inbox Queue
	void AddToInbox(FString* Message);
	//Pointer to Inbox Queue to store the Received Messages
	TQueue<FString*, EQueueMode::Mpsc>* Inbox;
	//States if the thread is finish
	bool isFinished;
	//Reference to the socket that communicates with the server
	SOCKET* conn_socket;
	//Lenght of the buffer
	int bufferLen = DEFAULT_BUFLEN;
	//Char buffer 
	char buffer[DEFAULT_BUFLEN];
	//Point to TCP connection class -> states if connection is established
	bool* isConnected;
};
