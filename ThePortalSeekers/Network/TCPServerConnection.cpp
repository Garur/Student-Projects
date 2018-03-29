// Fill out your copyright notice in the Description page of Project Settings.

#include "TCPServerConnection.h"
#include <sstream>
#include <string>

TCPServerConnection::TCPServerConnection(char* _servername, int _serverPort, int _localPort)
{
	servername = _servername;
	InitializeWinsock();
	CreateSocket();
	Connect();
	serverPort = _serverPort;
	localPort = _localPort;
}

TCPServerConnection::~TCPServerConnection()
{
	Shutdown();
}

SOCKET* TCPServerConnection::GetSocket()
{
	return &conn_socket;
}


char* TCPServerConnection::InitializeWinsock()
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

char* TCPServerConnection::CreateSocket()
{
	struct hostent *hp;
	hp = gethostbyname(servername);
	if (!hp) {
		return "Failed to get server address";
	}

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr = *((struct in_addr *)hp->h_addr);
	serverAddress.sin_port = htons(serverPort);
	memset(&(serverAddress.sin_zero), '\0', 8);

	//UE_LOG(LogTemp, Warning, TEXT("Create Receive Socket"));
	conn_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (conn_socket == INVALID_SOCKET) {
		//UE_LOG(LogTemp, Warning, TEXT("Socket creation failed"));
		m_isConnected = false;
		WSACleanup();
		return "Socket creation failed";
	}	
	//Set socket to allow multiple connections, otherwise the multiplayer does not start afterwards
	if (setsockopt(conn_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
	{
		return "setsockopt failed";
	}
	
	return "Socket created successful";
	
}

int TCPServerConnection::BindLocalPort()
{
	localAddress.sin_family = AF_INET;
	localAddress.sin_addr.s_addr = INADDR_ANY;
	localAddress.sin_port = htons(localPort);
	memset(&(localAddress.sin_zero), '\0', 8);

	if (bind(conn_socket, (struct  sockaddr *) &localAddress, sizeof(localAddress)) == SOCKET_ERROR)
	{
		return localPort;
		//return WSAGetLastError();
	}
	else
	{
		return 0;
	}
}


char* TCPServerConnection::Connect()
{
	if (connect(conn_socket, (struct  sockaddr *) &serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		return "Connect failed";
	}
	else {
		m_isConnected = true;
		return "Connect successful";
	}
}

bool* TCPServerConnection::IsConnected()
{
	return &m_isConnected;
}
char* TCPServerConnection::GetLocalAddresses()
{	
	char name[256];	
	PHOSTENT hostinfo;
	char *ip;
	memset(buffer, 0, bufferLen);
	if (gethostname(name, sizeof(name)) == 0)
	{
		//printf("Host name: %s\n", name);
		if ((hostinfo = gethostbyname(name)) != NULL)
		{
			int nCount = 0;
			while (hostinfo->h_addr_list[nCount])
			{
				ip = inet_ntoa(*(struct in_addr *)hostinfo->h_addr_list[nCount]);
				++nCount;
				strcat(buffer, ip);	
				strcat(buffer, ",");
			}			
		}		
	}
	return buffer;
}

int TCPServerConnection::GetLocalPort()
{
	return localPort;
}

int TCPServerConnection::Receive(char **OutMessage)
{
	memset(buffer, 0, bufferLen);
	//UE_LOG(LogTemp, Warning, TEXT("Trying to Receive"));
	iResult = recv(conn_socket, buffer, bufferLen, 0);

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
		m_isConnected = false;
		closesocket(conn_socket);
		WSACleanup();
		return iResult;
	}
	//return iResult;
}

int TCPServerConnection::Send(char* _message)
{	
	int iSendResult = send(conn_socket, _message, strlen(_message), 0);
	if (iSendResult == SOCKET_ERROR) {		
		m_isConnected = false;
		closesocket(conn_socket);
		WSACleanup();
		//return "Sent failed";
	}
	//Returns the number of bytes sent
	return iSendResult;
	
}

void TCPServerConnection::Shutdown()
{
	//UE_LOG(LogTemp, Warning, TEXT("shutdown"));
	//Close Receive Socket
	int iResult = shutdown(conn_socket, SD_BOTH);
	if (iResult == SOCKET_ERROR) {
		//UE_LOG(LogTemp, Warning, TEXT("shutdown failed %i"), iResult);
		m_isConnected = false;
		closesocket(conn_socket);
		WSACleanup();
		return;
	}
	closesocket(conn_socket);
	WSACleanup();
}
