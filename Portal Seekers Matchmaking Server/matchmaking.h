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
#include "database_conn.h"

#define TRUE   1
#define FALSE  0
#define BUFFER_SIZE 512
#define DB_NAME "portal_seekers_database.db"
#define RELAY_OPT 0 //Enables and disables relay by passing the server address to all the peers as an option to connect 

/*
 * Class that handles all the network communication and matchmaking functionality 
*/
class Matchmaking {
public:
	Matchmaking(Room_Manager* room_manger);
	~Matchmaking();
	//Sets the port to listen to incoming messages
	void SetPort(int _port);
	// Initializes all the necessary methods needed for the network communication
	void Initialize();
	// Refreshes the socket set needed for the selector 
	void RefreshSockets();
	//Calls Selector method that waits for activity in all the sockets
	void WaitForActivity();
	//Handles connections received in the master socket
	void MasterSocketActivity();
	//Handle connections received in all other sockets
	void SocketsActivity();
	//Broadcast the neccessary information to all the peers for the current game 
	void StartGame();
	//Broadcast message to all connected peers
	void BroadcastMessage(char* _msg);
	void BroadcastMessage(const char* _msg);
	//Sets the number of players that are need for the game to start
	void SetPlayersPerGame(int _playersPerGame);

private:		
	//Handles the received message
	bool IsReceivedMessageValid(int socket);
	//Handle new Player Connection
	void HandleNewPlayerConnection();
	//Update Player Counter
	void UpdatePlayerCounter();
	// Validate the message received
	bool IsMessageValid(std::vector<char*> _split_message);
	// Analise and takes the needed action related to the message 
	void ParseReceivedMessages(int _socket);
	//Prints Eror message
	void error(const char *msg);	
	//Utility function that converts values to strings
	template <typename T>	
	std::string to_string(T value);
	//Close and clean the specified socket
	void CloseSocket(int socket);

	//Instance of the room manager
	Room_Manager* m_room_manager;
	//Instance of Database Connection 
	Database_Conn* m_db_conn;

	int opt = TRUE; 
	int master_socket;
	int port, addrlen, new_socket, client_socket[30], max_clients = 30, activity;
	int max_sd, socket_itr;
	int playerCounter = 0;
	//Number of players that each game has
	int playersPerGame;
	//set of socket descriptors to monitor
	fd_set readfds;
	//Local address
	struct sockaddr_in address; 
	//Vector list containing the current game addresses
	//std::vector<sockaddr_in> peerAdresses = std::vector<sockaddr_in>();
	//Buffer to store received messages and messages to send
	char buffer[BUFFER_SIZE];  //data buffer of 512B
	//Welcoming message
	char *welcome_message = "Connected to the portal seekers server;";

	std::vector<std::vector<char*>> messages_packet;



};

enum OP_CODES
{
	OP_NULL = 0,
	OP_PLAY = 1, //initial message received by players who want to play
	OP_ID = 2, //Message contains ID and number of players
	OP_IP = 3, //messages received with this code contain ips of the peers
	OP_INFO = 4, //info messages sent by the server 
	OP_FINISH = 5 //Informs the server that the game has finished
};



