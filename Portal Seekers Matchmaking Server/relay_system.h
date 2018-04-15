#pragma once
#include <stdio.h>
#include <string.h>   
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   
#include <arpa/inet.h>    
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <vector>
#include <string>
#include <iostream>
#include "room_manager.h"

#define TRUE   1
#define FALSE  0
#define BUFFER_SIZE 512

/*
* Class that handles all UDP network communication with the clients
*/
class RelaySystem
{
public:
	RelaySystem(Room_Manager* room_manager);
	~RelaySystem();
	
	//Sets the port to listen to incoming messages
	void SetPort(int _port);
	// Initializes all the necessary methods needed for the network communication using UDP
	void Initialize();	
	//Handles connections received in the master socket
	void MasterSocketActivity();

private:
	bool ValidateIp(sockaddr_in address);
	//Handles the received message
	void ForwardMessage(sockaddr_in address);
	//Prints Eror message
	void error(const char *msg);

	Room_Manager* room_manager;

	int opt = TRUE;
	int master_socket;
	int port, addrlen;
	int max_sd, socket_itr;
	//Local address
	struct sockaddr_in local_address;
	//Received Connection Address Container
	struct sockaddr_in recv_address;
	//Received Address length
	unsigned int recv_len = sizeof(recv_address);
	//Vector list containing the current game addresses
	std::vector<sockaddr_in> peerAdresses = std::vector<sockaddr_in>();
	//Buffer to store received messages and messages to send
	char buffer[BUFFER_SIZE];  //data buffer of 1K	
	//Welcoming message
	char *message = "Connected to the portal seekers server;";
};

