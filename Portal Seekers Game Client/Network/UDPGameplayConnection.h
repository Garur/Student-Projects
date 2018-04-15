// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include<stdio.h>
#include <winsock2.h>
#include <Ws2tcpip.h> 
#include <string>
#include <vector>

#define DEFAULT_BUFLEN 512

/**
* Responsible to create UDP gameplay connection, sends and receives messages with all the peers
* Uses the Winsock library
*/
class UDPGameplayConnection
{
public:
	UDPGameplayConnection(std::vector<std::string> _peersAdrresses, int _local_port, int*_ports);
	~UDPGameplayConnection();
	//Receives Messages through actives connection
	int Receive(char** OutMessage, bool isTesting);
	//Send Message through active connection
	char* Send(char* Message);
	//Shuts down connection
	void Shutdown();
	//Initializes Winsock
	char* InitializeWinsock(); 
	//Creates Socket
	char* CreateSocket();	
	//Setter Method for peersNames and ports
	char* SetPeersAddresses(std::vector<std::string> _peersAdrresses, int*_ports);
	//Binds the Receiving Socket 
	char* BindReceivingSocket();
	//states if sockets are active and connected
	bool* IsConnected();
	//Getter Method for the local Address
	sockaddr_in* GetLocalAddress();
	//Setter method for the localAddress in case it needs to be copied from another connection (returns error/ success message)
	char* CopyLocalAddress(sockaddr_in * _localAddress);
	//Getter for the conn_socket variable
	SOCKET* GetSocket();
	//Setter for conn_socket variable (used to copy the socket from the UDPRecvThread's UDPconnection to UDPSenderThread's UDPConnection
	char* CopySocket(SOCKET* _socket);


protected:
	//Setup peers address array with information from peerNames. Returns error or success message
	char* CreatePeersAddresses();
	//Compares the ip address with the list of ip addresses that the class has
	bool ValidateIPAddress(sockaddr_in address);
	//Array with the adresses names of all the peers
	std::vector<std::string> peersNames;	
	//Socket
	SOCKET conn_socket; 
	//Local Address
	struct sockaddr_in localAddress;
	//port to be used in the connection both to receive and send
	int local_port; 
	//Container for the address of all received messages
	struct sockaddr_in peer_Address; 
	int peer_len = sizeof(peer_Address);
	//Vector containing all other peer's addresses
	std::vector<sockaddr_in> peerAdresses = std::vector<sockaddr_in>();	
			
	//Array containing all the ports of the peers
	int* ports; 
	//Lenght of the buffer
	int buflen = DEFAULT_BUFLEN;
	//Buffer for received or sent messages
	char buffer[DEFAULT_BUFLEN];
	//Code resulting from winsock functionss
	int iResult = 0; 
	//States if the connection is active
	bool m_isConnected = false;
	// Option Value for the setsockopt()
	int opt = 1;
};




