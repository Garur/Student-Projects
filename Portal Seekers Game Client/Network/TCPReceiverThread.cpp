// Fill out your copyright notice in the Description page of Project Settings.

#include "TCPReceiverThread.h"
#include <RunnableThread.h>
#include "PlatformProcess.h"
#include "Containers/UnrealString.h"
#include "ConfigCacheIni.h"

TCPReceiverThread* TCPReceiverThread::Runnable = NULL;

TCPReceiverThread::TCPReceiverThread(TQueue<FString*, EQueueMode::Mpsc>* _Inbox, SOCKET* _socket, bool* _isConnected) : StopTaskCounter(0)
{
	isFinished = false;
	Inbox = _Inbox;
	conn_socket = _socket;
	isConnected = _isConnected;
	Thread = FRunnableThread::Create(this, TEXT("TCPThreadReceiver"), 0, TPri_BelowNormal);
}

TCPReceiverThread::~TCPReceiverThread()
{
	UE_LOG(LogTemp, Warning, TEXT("Close Thread"));
	delete Thread;
	Thread = NULL;
}


TCPReceiverThread * TCPReceiverThread::JoyInit(TQueue<FString*, EQueueMode::Mpsc>* _Inbox, SOCKET* _socket, bool* _isConnected)
{
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new TCPReceiverThread(_Inbox, _socket, _isConnected);
	}
	return Runnable;
}

bool TCPReceiverThread::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("Init Receiver"));
	return true;
}

uint32 TCPReceiverThread::Run()
{
	UE_LOG(LogTemp, Warning, TEXT("Run TCP Receiver"));
	//Initial wait before starting
	FPlatformProcess::Sleep(0.03);
	while (StopTaskCounter.GetValue() == 0 && !isFinished)
	{		
		if (*isConnected) {
			//UE_LOG(LogTemp, Warning, TEXT("Trying Receiver"));
			char* CharMessage = nullptr;
			//Call receive Message from UDPReceiver Connection Class 
			int iResult = Receive(&CharMessage);
			//After receiving -> check the result
			if (iResult > 0) {
				if (CharMessage == nullptr) {
					UE_LOG(LogTemp, Error, TEXT("Message Receive is null"));
				}
				else {
					TArray<FString> SplitMessage;
					FString* RecvMessage = new FString(CharMessage);
					int32 someint = RecvMessage->ParseIntoArray(SplitMessage, TEXT(";"), false);
					for (int i = 0; i < SplitMessage.Num() - 1; i++) {
						UE_LOG(LogTemp, Warning, TEXT("Thread Receiver Message %d received: %s"), i, *SplitMessage[i]);						
						//Adds Message to Inbox Queue
						AddToInbox(new FString(SplitMessage[i]));
					}					
				}
			}
			else {
				isFinished = true;
				Shutdown();
				UE_LOG(LogTemp, Warning, TEXT("Receive failed: %i"), WSAGetLastError());
			}
		}	
	}
	return 0;
}


int TCPReceiverThread::Receive(char **OutMessage)
{
	memset(buffer, 0, bufferLen);
	//UE_LOG(LogTemp, Warning, TEXT("Trying to Receive"));
	int iResult = recv(*conn_socket, buffer, bufferLen, 0);

	if (iResult > 0) {
		//UE_LOG(LogTemp, Warning, TEXT("Bytes received: %d\n"), iResult);	
		*OutMessage = buffer;
	}
	else if (iResult == 0) {
		//UE_LOG(LogTemp, Warning, TEXT("Connection closing"));
		UE_LOG(LogTemp, Warning, TEXT("Stopped receiving"));
		//isFinished = true;
		//Shutdown();
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("TCP Receiver failed failed %i"), WSAGetLastError());
		//isFinished = true;
		//Shutdown();
	}
	return iResult;
}


void TCPReceiverThread::Stop()
{
	UE_LOG(LogTemp, Warning, TEXT("TCP Receiver Stop"));
	StopTaskCounter.Increment();
}

void TCPReceiverThread::EnsureCompletion()
{
	UE_LOG(LogTemp, Warning, TEXT("TCP Receiver EnsureCompletion"));
	Stop();
	Thread->WaitForCompletion();
}

void TCPReceiverThread::Shutdown()
{
	UE_LOG(LogTemp, Warning, TEXT("TCP Receiver Shutdown"));
	if (Runnable)
	{
		Runnable->EnsureCompletion();
		delete Runnable;
		Runnable = NULL;
	}
}

bool TCPReceiverThread::IsThreadFinished()
{
	if (Runnable) return Runnable->isFinished;
	return true;
}

void TCPReceiverThread::AddToInbox(FString* Message)
{
	//UE_LOG(LogTemp, Warning, TEXT("Add message to queue: %s"), **Message);
	Inbox->Enqueue(Message);
}
