#include "relay_system.h"

RelaySystem::RelaySystem(Room_Manager* room_manager) : room_manager(room_manager)
{

}


RelaySystem::~RelaySystem()
{

}



void RelaySystem::SetPort(int _port) {
	//Set port number
	port = _port;
	if (port == 0) {
		error("Please especify a valid port number");
	}
}

void RelaySystem::Initialize()
{
	//create a master socket
	if ((master_socket = socket(AF_INET, SOCK_DGRAM, 0)) == 0)
	{
		error("socket failed");
	}

	//set master socket to allow multiple connections , this is just a good habit, it will work without this
	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
	{
		error("setsockopt");
	}

	//type of socket created
	local_address.sin_family = AF_INET;
	local_address.sin_addr.s_addr = INADDR_ANY;
	local_address.sin_port = htons(port);

	//bind the socket to localhost port 8888
	if (bind(master_socket, (struct sockaddr *)&local_address, sizeof(local_address))<0)
	{
		error("bind failed");
	}
		
	addrlen = sizeof(local_address);
	puts("Waiting for connections in UDP socket...");

}

void RelaySystem::MasterSocketActivity()
{		
		
		//Reset Buffer 
		memset(buffer, 0, BUFFER_SIZE);
		
		//Read incoming message
		int result = recvfrom(master_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*) &recv_address, &recv_len);
		if (result < 0) error("ERROR reading from socket");
			
		// Verify and Parse the Message		
		bool isValid = ValidateIp(recv_address);
		//If connection is not valid return
		if (!isValid) {
			return;
		}

		ForwardMessage(recv_address);	
		
	
}

bool RelaySystem::ValidateIp(sockaddr_in address)
{
	return true;
}

void RelaySystem::ForwardMessage(sockaddr_in address)
{
	//printf("Message Received in UDP socket: %s from %s %d \n", buffer, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
	// find the room where the peer with this address is 
	Room* room = room_manager->GetRoomByAddress(address);
	sockaddr_in peer_address;
	
	if (room == nullptr) {
		printf("No room found with that address \n");
		return;
	}
	//Loop through all the players in the room
	for (int i = 0; i < room->players.size(); i++) {
		peer_address = room->players[i]->main_address;
		//Checks if this was the  peer that sent the message, if true, continue to the next iteration
		if (address.sin_addr.s_addr == peer_address.sin_addr.s_addr && address.sin_port == peer_address.sin_port) {	
			//printf("Not sent to %s %d \n", inet_ntoa(peer_address.sin_addr), ntohs(peer_address.sin_port));
			continue;
		}
		//printf("Sent to %s %d \n", inet_ntoa(peer_address.sin_addr), ntohs(peer_address.sin_port));
		//send new connection greeting message
		if (sendto(master_socket, buffer, strlen(buffer), 0, (struct sockaddr*) &peer_address, sizeof(peer_address)) != strlen(buffer))
		{
			error("sendto");
		}
		else {
			//printf("Message forward: %s", buffer);
		}
		
	}	
}

void RelaySystem::error(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}