// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include<stdio.h>
#include <string>
#include <vector>
#include <winsock2.h>
#include <Ws2tcpip.h> 

#define DEFAULT_BUFLEN 512

/**
 * Responsible to create the connection with the server
 * Uses the Winsock library
 */
class TCPServerConnection
{
public:
	TCPServerConnection(char* _servername, int _serverPort, int _localPort);
	~TCPServerConnection();	
	//Getter Method for the socket created that is used in the connection
	SOCKET* GetSocket();
	//Encapsulation of Winsock Receive method
	int Receive(char** OutMessage);
	//Encapsulation of Winsock Send method
	int Send(char* Message);
	//Initializes Winsock
	char* InitializeWinsock();
	//Create Socket
	char* CreateSocket();
	//Connect to server
	char* Connect();
	//Give the communication local port a static value (important for debug in the same computer) 
	int BindLocalPort();
	//States if connection is established with the server
	bool* IsConnected();
	//Retrives all the local addresses that this client onws
	char* GetLocalAddresses();
	//Getter for the port number of this client
	int GetLocalPort();
	
private:
	//Shuts down the connection
	void Shutdown();
	//Referent to the socket used to communicate with the server
	SOCKET conn_socket;
	//Addresses
	struct sockaddr_in serverAddress, localAddress;
	//Name of the server
	char* servername;
	//Port of the Server to connect with
	int serverPort;
	//Local Port
	int localPort;
	//Option used in setsockopt()
	int opt = 1; 
	//Buffer Lenght
	int bufferLen = DEFAULT_BUFLEN;
	//Char Buffer
	char buffer[DEFAULT_BUFLEN];
	//Error Code resulting from the winsock methods
	int iResult = 0;
	//States if the connection is active
	bool m_isConnected = false;

};
