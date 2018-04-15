// Fill out your copyright notice in the Description page of Project Settings.

#include "UDPGameplayConnection.h"
#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib,"ws2_32.lib")

UDPGameplayConnection::UDPGameplayConnection(std::vector<std::string> _peersNames, int _local_port, int* _ports) : peersNames(_peersNames), local_port(_local_port), ports(_ports)
{
	CreatePeersAddresses();
}

UDPGameplayConnection::~UDPGameplayConnection()
{
	Shutdown();
}

char* UDPGameplayConnection::InitializeWinsock()
{
	//Start up WinSock
	WSADATA info;
	if (WSAStartup(MAKEWORD(2, 2), &info) != 0) {
		return "Winsock startup failed";
	}
	else {
		return "Winsock started successful";
	}
}

char* UDPGameplayConnection::CreateSocket()
{			
	localAddress.sin_family = AF_INET;
	localAddress.sin_addr.s_addr = INADDR_ANY;
	localAddress.sin_port = htons(local_port);
	memset(&(localAddress.sin_zero), '\0', 8);
	
	conn_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (conn_socket == INVALID_SOCKET) {
		WSACleanup();
		return "UDP Receive Socket creation failed";
	}
	
	//set allow multiple connections , this is just a good habit
	if (setsockopt(conn_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
	{
		memset(buffer, 0, buflen);
		int error_code = WSAGetLastError();
		sprintf(buffer, "setsockopt failed: %d", error_code);
		return buffer;
	}
	else {
		memset(buffer, 0, buflen);
		int port_num = (int)ntohs(localAddress.sin_port);
		char* t_address = inet_ntoa(localAddress.sin_addr);
		sprintf(buffer, "Local Receiving Socket created successful. On Address %s:%d", t_address, port_num);
		return buffer;
	}
	
}

char* UDPGameplayConnection::BindReceivingSocket()
{
	//Reset Buffer
	memset(buffer, 0, buflen);
	//UE_LOG(LogTemp, Warning, TEXT("Bind Receive Socket"));
	if (bind(conn_socket, (struct  sockaddr *) &localAddress, sizeof(localAddress)) == SOCKET_ERROR)
	{		
		m_isConnected = false;
		int error_code = WSAGetLastError();
		sprintf(buffer, "Failed to bind recv local socket. Error code %d", error_code);
		return buffer;
	}
	else
	{
		m_isConnected = true;
		int port_num = (int)ntohs(localAddress.sin_port);
		sprintf(buffer, "Bind successfull. Port binded: %d", port_num);
		return buffer;
	}
}

bool * UDPGameplayConnection::IsConnected()
{
	return &m_isConnected;
}

sockaddr_in* UDPGameplayConnection::GetLocalAddress()
{
	return &localAddress;
}

char* UDPGameplayConnection::CopyLocalAddress(sockaddr_in * _localAddress)
{	
	localAddress = *_localAddress;
	return "Address copied";
}

SOCKET * UDPGameplayConnection::GetSocket()
{
	return &conn_socket;
}

char* UDPGameplayConnection::CopySocket(SOCKET * _socket)
{
	conn_socket = *_socket;
	return "Socket copied";
}

int UDPGameplayConnection::Receive(char **OutMessage, bool isTesting)
{
	memset(buffer, 0, buflen);
	//UE_LOG(LogTemp, Warning, TEXT("Trying to Receive"));
	iResult = recvfrom(conn_socket, buffer, buflen, 0, (struct sockaddr*) &peer_Address, &peer_len);
	
	//If connections are being tested
	if (isTesting) {
		//Add local ip address to the message		
		strcat(buffer, ":");
		char* address = inet_ntoa(peer_Address.sin_addr);
		strcat(buffer, address);
		//int port = (int)ntohs(localAddress.sin_port);
		//char* c_port = &std::to_string(port).c_str;
		//strcat(sendbuf, c_port);
	}		
	/*
	if (!ValidateIPAddress(peer_Address)) {
		*OutMessage = recvbuf;	
		return 0;
	}*/

	if (iResult > 0) {
		//UE_LOG(LogTemp, Warning, TEXT("Bytes received: %d\n"), iResult);	
		*OutMessage = buffer;
		return iResult;
	}
	else if (iResult == 0) {
		//UE_LOG(LogTemp, Warning, TEXT("Connection closing"));
		return iResult;
	}
	else {
		//UE_LOG(LogTemp, Warning, TEXT("recv failed %i"), WSAGetLastError());
		//m_isConnected = false;
		//closesocket(conn_socket);
		//WSACleanup();		
		return WSAGetLastError();
	}
}

char * UDPGameplayConnection::SetPeersAddresses(std::vector<std::string> _peerNames, int * _ports)
{
	peersNames.clear();
	peersNames = _peerNames;
	ports = _ports;
	CreatePeersAddresses();
	return nullptr;
}


char* UDPGameplayConnection::Send(char* Message)
{
	memset(buffer, 0, buflen);
	strcat(buffer, Message);
	//Replicate every message sent to all the peers
	for (int i = 0; i < peerAdresses.size(); i++) {
		int iSendResult = sendto(conn_socket, buffer, strlen(buffer), 0, (struct sockaddr *)&peerAdresses[i], sizeof(peerAdresses[i]));
		if (iSendResult == SOCKET_ERROR) {
			m_isConnected = false;
			closesocket(conn_socket);
			WSACleanup();
			return "Sent failed";
		}
	}
	//Return the message sent for debug purposes
	return buffer;
}

void UDPGameplayConnection::Shutdown()
{
	//Close Receive Socket
	int iResult = shutdown(conn_socket, SD_BOTH);
	//If Failled
	if (iResult == SOCKET_ERROR) {
		m_isConnected = false;
		closesocket(conn_socket);
		WSACleanup();
		return;
	}
	closesocket(conn_socket);
	WSACleanup();
}

char* UDPGameplayConnection::CreatePeersAddresses()
{
	//Reset the array with the adresses of all the peers		
	peerAdresses.clear();
	for (int i = 0; i < peersNames.size(); i++) {
		struct sockaddr_in newAddress;
		peerAdresses.push_back(newAddress);
	}

	//Set up PeerAddresses	
	for (int i = 0; i < peersNames.size(); i++) {
		//Declare variable of type ‘struct hostent’ 
		struct hostent *host;
		host = gethostbyname(peersNames[i].c_str());
		if (!host) {
			return "UDP Send: Failed to get peer name";
		}
		peerAdresses[i].sin_family = AF_INET;
		peerAdresses[i].sin_addr = *((struct in_addr *)host->h_addr);
		peerAdresses[i].sin_port = htons(ports[i]);
		memset(&(peerAdresses[i].sin_zero), '\0', 8);
	}

	memset(buffer, 0, buflen);
	sprintf(buffer, "Peer addresses successfully set");
	return buffer;
}


bool UDPGameplayConnection::ValidateIPAddress(sockaddr_in address)
{
	for (int i = 0; i < peerAdresses.size(); i++) {
		if (address.sin_addr.s_addr == peerAdresses[i].sin_addr.s_addr) {
			return true;
		}
	}
	return false;
}




