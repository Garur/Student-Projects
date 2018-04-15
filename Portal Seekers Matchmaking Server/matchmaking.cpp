#include "matchmaking.h"
#include <sstream>



Matchmaking::Matchmaking(Room_Manager* room_manager) : m_room_manager(room_manager) {
		
	m_db_conn = new Database_Conn(DB_NAME);
}

Matchmaking::~Matchmaking()
{
}

void Matchmaking::SetPort(int _port) {
	//Set port number
	port = _port;
	if (port == 0) {
		error("Please especify a valid port number");
	}
}

void Matchmaking::Initialize()
{
	//initialise all client_socket[] to 0 so not checked
	for (int i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}
	//create a master socket
	if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		error("socket failed");
	}

	//set master socket to allow multiple connections , this is just a good habit, it will work without this
	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
	{
		error("setsockopt");
	}

	//type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	//bind the socket to localhost port 8888
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
	{
		error("bind failed");
	}
	printf("Listening on port %d \n", port);
	//try to specify maximum of 10 pending connections for the master socket
	if (listen(master_socket, 10) < 0)
	{
		error("listen");
	}
	//accept the incoming connection
	addrlen = sizeof(address);
	puts("Waiting for connections ...");

}

void Matchmaking::RefreshSockets() {
	//clear the socket set
	FD_ZERO(&readfds);
	//add master socket to set
	FD_SET(master_socket, &readfds);
	max_sd = master_socket;
	//add child sockets to set
	for (int i = 0; i < max_clients; i++)
	{
		//socket descriptor
		int socket_itr = client_socket[i];

		//if valid socket descriptor then add to read list
		if (socket_itr > 0)
			FD_SET(socket_itr, &readfds);
		//updates highest file descriptor number, need it for the select function
		if (socket_itr > max_sd)
			max_sd = socket_itr;
	}
}

void Matchmaking::WaitForActivity()
{
	//wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
	activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

	if ((activity < 0) && (errno != EINTR))
	{
		printf("select error");
	}
}

void Matchmaking::MasterSocketActivity()
{
	//If something happened on the master socket , then its an incoming connection
	if (FD_ISSET(master_socket, &readfds))
	{
		if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
		{
			error("accept");
		}

		//inform user of socket number - used in send and receive commands
		printf("New connection , socket fd is %d , ip is : %s , port : %d \n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));		
		
		//send new connection greeting message
		if (send(new_socket, welcome_message, strlen(welcome_message), 0) != strlen(welcome_message))
		{
			perror("send");
		}

		puts("Welcome message sent successfully");

		HandleNewPlayerConnection();		
	}
}

void Matchmaking::HandleNewPlayerConnection()
{
	//add new socket to array of active sockets
	for (int i = 0; i < max_clients; i++)
	{
		//if position in array is empty
		if (client_socket[i] == 0)
		{
			//save the socket 
			client_socket[i] = new_socket;
			puts("Added new connection to list of sockets");
			//When the socket is saved on the first available position, break the loop
			break;
		}
	}
}

void Matchmaking::SocketsActivity()
{
	//Check for activity in other sockets
	for (int i = 0; i < max_clients; i++)
	{
		socket_itr = client_socket[i];

		if (FD_ISSET(socket_itr, &readfds))
		{
			//printf("Socket Activity \n");
			//Reset Buffer 
			memset(buffer, 0, BUFFER_SIZE);
			//Read Message
			int valread = read(socket_itr, buffer, BUFFER_SIZE);
			
			//Check if it was for closing , and also read the incoming message
						
			if (valread == 0)
			{
				//Somebody disconnected , get his details and print
				getpeername(socket_itr, (struct sockaddr*)&address, (socklen_t*)&addrlen);
				printf("Host disconnected , ip %s , port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
				
				//Get the room that the player was in
				Room* room = m_room_manager->GetRoomByAddress(address);
				
				//If the room of the player is the current room 
				if (m_room_manager->IsCurrentRoom(room)) {
					//Decrease player counter 
					playerCounter--;
					//Remove PLayer from the room
					room->RemovePlayer(address);
				}
				//If player was in a game and game is still on, warn the other players.
				room->PlayerDisconnected();


				//Close the socket and mark as 0 in list for reuse
				close(socket_itr);
				client_socket[i] = 0;
			}
			else if (valread > 0)
			{
				// Verify the Received Message
				if (!IsReceivedMessageValid(socket_itr)) {
					return;
				}			

				//Else Accept the connection
				BroadcastMessage("4:Another player joined;");
				//Parse the message received to store the inner ip address of the client
				ParseReceivedMessages(socket_itr);
				//Update the player counter
				UpdatePlayerCounter();

			}
			else {			
				//Print Error Message
				fprintf(stderr, "%s\n", strerror(errno));
			}
		}
	}
}

bool Matchmaking::IsReceivedMessageValid(int _socket)
{
	//Clean the vector to store the messages received in the last packet
	messages_packet.clear();
	//Split the packet in individual messages to process
	//Vector to be filled with individual messages
	std::vector<char*> split_patcket;
	char* char_message = strtok(buffer, ";");
	while (char_message)
	{
		split_patcket.push_back(char_message);
		char_message = strtok(NULL, ";");
	}	
	//Split the messages into arguments and store it in an array
	std::vector<char*> split_message;
	for (int i = 0; i < split_patcket.size(); i++) {
		split_message.clear();
		printf("Message Received: %s\n", split_patcket[i]);
		char* chars_array = strtok(split_patcket[i], ":");
		while (chars_array)
		{
			split_message.push_back(chars_array);
			chars_array = strtok(NULL, ":");
		}

		//Saves the messages for later use;
		messages_packet.push_back(split_message);

		//If the operation code is not correct or if the number of arguments is wrong
		if (!IsMessageValid(split_message)) {
			char* error_msg = "Computer says no! \n";
			//Reply with an error message
			if (send(_socket, error_msg, strlen(error_msg), 0) != strlen(error_msg))
			{
				perror("send");
			}
			//Close socket
			CloseSocket(_socket);
			puts("Unauthorized connection discarded");
			return false;
		}
	}
	return true;
}

bool Matchmaking::IsMessageValid(std::vector<char*> _split_message)
{
	//Get OP code
	int op_code = atoi(_split_message[0]);

	//Validate message for each operation code
	if (op_code == OP_FINISH && _split_message.size() == 4) {
		return true;
	}
	else if (op_code == OP_PLAY && _split_message.size() == 3) {
		return true;
	}
	else if (op_code == OP_INFO && _split_message.size() == 2) {
		return true;
	}
	else {
		return false;
	}
}

void Matchmaking::BroadcastMessage(char * _msg)
{
	//Broadcast to all peers that one more peer joined	
	for (int i = 0; i < max_clients; i++)
	{
		//if there is someone connected to socket i
		if (client_socket[i] != 0)
		{
			if (send(client_socket[i], _msg, strlen(_msg), 0) != strlen(_msg))
				perror("send");
		}
	}
}

void Matchmaking::BroadcastMessage(const char * _msg)
{
	//Broadcast to all peers that one more peer joined	
	for (int i = 0; i < max_clients; i++)
	{
		//if there is someone connected to socket i
		if (client_socket[i] != 0)
		{
			if (send(client_socket[i], _msg, strlen(_msg), 0) != strlen(_msg))
				perror("send");
		}
	}
}

void Matchmaking::ParseReceivedMessages(int _socket)
{
	for (int i = 0; i < messages_packet.size(); i++) {
		//Get OP code
		int op_code = atoi(messages_packet[i][0]);
		//Validate message for each operation code
		if (op_code == OP_FINISH) {
			//Get the value for the winner of the game
			int winner = atoi(messages_packet[i][1]);
			//Save game statistics in the database
			m_db_conn->SaveGameRecord(winner, messages_packet[i][2], messages_packet[i][3]);
			//Close all the connections
			int* sockets = m_room_manager->GetAllSocketsOfRoomBySocket(_socket);
			for (int j = 0; j < playersPerGame; j++) {
				//Close socket
				CloseSocket(sockets[j]);
			}
			//close room
			m_room_manager->CloseRoom(_socket);
		}
		else if (op_code == OP_PLAY) {

			//Get the address linked to the new socket
			getpeername(socket_itr, (struct sockaddr*)&address, (socklen_t*)&addrlen);
			//Add new Connection to a room
			m_room_manager->AddNewConnection(address, socket_itr);

			
			//Get the value of the inner address			
			//split this -> messages_packet[i][1] 
			std::vector<char*> split_addresses;

			char* chars_array = strtok(messages_packet[i][1], ",");
			while (chars_array)
			{
				split_addresses.push_back(chars_array);
				chars_array = strtok(NULL, ",");
			}
			//Saves all the addresses received 
			sockaddr_in inner_Address;
			for (int j = 0; j < split_addresses.size(); j++) {
				inet_aton(split_addresses[j], &inner_Address.sin_addr);
				//converts the received string port and stores it in the address struct
				inner_Address.sin_port = htons(atoi(messages_packet[i][2]));
				//Store the inner address of the player that sent the message
				m_room_manager->SavePlayerAddress(address, inner_Address);
			}
			if (RELAY_OPT) {
				//Also add server address to the list 
				inet_aton("151.236.220.247", &inner_Address.sin_addr);
				inner_Address.sin_port = htons(55001);
				m_room_manager->SavePlayerAddress(address, inner_Address);
			}
		}
	}
}

void Matchmaking::UpdatePlayerCounter()
{

	//Connection was accepted -> increases the playerCounter
	playerCounter++;
	printf("Players count: %d Players per game: %d\n", playerCounter, playersPerGame);	
	// When the player counter reachs the number of players needed to start the game
	if (playerCounter == playersPerGame) {
		//Start the game
		StartGame();
		//reset player counter
		playerCounter = 0;
	}
}

void Matchmaking::StartGame()
{
	puts("We have enough players to start a game\n");
	//Set room is playing
	int playerID = 0;
	const char* num;
	//Reset Buffer
	memset(buffer, 0, sizeof(BUFFER_SIZE));

	std::vector<Player*> players = m_room_manager->GetCurrentRoom()->players;
	//Iterate through all the sockets
	for (int i = 0; i < players.size(); i++)
	{
		socket_itr = players[i]->socket; 
		//if this socket is used
		if (socket_itr != 0)
		{
			//Build ID Message - Example = "2:1:2\0";					
			strcat(buffer, "2:");//Add op code					
			num = to_string(playerID).c_str();
			strcat(buffer, num);//Add id to message		

			//For testing purposes, allows the server to count as a 2nd player if games are setup for only having one player
			if (players.size() == 1) {
				num = "2";
			}
			else {
				num = to_string(players.size()).c_str();
			}
			strcat(buffer, ":");
			strcat(buffer, num); //Add number of players to message			
			strcat(buffer, ";\0"); //End message
			printf("ID Message: %s\n", buffer);
			//Send Message with ID and Num of players
			if (send(socket_itr, buffer, strlen(buffer), 0) != strlen(buffer)) {
				perror("send");
			}
			//Reset Buffer
			memset(buffer, 0, BUFFER_SIZE);


			//Send IP Message for each connected peer
			for (int j = 0; j < players.size(); j++) {
				if (i == j && players.size() != 1) {
					continue;
				}
				for (int k = 0; k < players[j]->addresses.size(); k++) {

					//Create IP Messages of addresses received in the server
					strcat(buffer, "3:");//op_code
					char *ip = inet_ntoa(players[j]->addresses[k].sin_addr);
					strcat(buffer, ip); //Add IP to the message
					strcat(buffer, ":");
					int port_num = ntohs(players[j]->addresses[k].sin_port);
					const char* player_port = to_string(port_num).c_str();
					strcat(buffer, player_port); //Add player port to the message 
					strcat(buffer, ":");
					const char* id = to_string(players[j]->id).c_str();
					strcat(buffer, id); //add the player ID associated with this ip
					strcat(buffer, ";\0");//finish message
					printf("IP Message: %s\n", buffer);
					//Send Message
					if (send(socket_itr, buffer, strlen(buffer), 0) != strlen(buffer))
						perror("send");
					//Reset Buffer
					memset(buffer, 0, BUFFER_SIZE);
				}
			}

			playerID++; // increments the player ID
			//Reset char arrays
			memset(buffer, 0, BUFFER_SIZE);
		}
	}
	//Informs the room manager that the room is full 
	m_room_manager->StartGame();
	playerCounter = 0;
}

void Matchmaking::SetPlayersPerGame(int _playersPerGame)
{
	if (_playersPerGame > 5) {
		error("Please especify a valid number of players. From 2 to 5.");
	}
	playersPerGame = _playersPerGame;
}

void Matchmaking::error(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

template<typename T>
std::string Matchmaking::to_string(T value)
{//create an output string stream
	std::ostringstream os;
	//throw the value into the string stream
	os << value;
	//convert the string stream into a string and return
	return os.str();
}


void Matchmaking::CloseSocket(int socket)
{
	for (int i; i < max_clients; i++) {
		if (socket == client_socket[i]) {
			client_socket[i] = 0;
		}
	}
	close(socket);
}