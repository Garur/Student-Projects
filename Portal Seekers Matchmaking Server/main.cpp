#include "matchmaking.h"
#include "relay_system.h"
#include <thread>
#include <sstream>

Matchmaking* matchmaking = nullptr;
RelaySystem* relay_system = nullptr;

void RunMatchmaking() {
	
	while (true)
	{
		matchmaking->RefreshSockets();
		matchmaking->WaitForActivity();
		matchmaking->MasterSocketActivity();
		matchmaking->SocketsActivity();
	}
}

void RunRelaySystem() {
	while (true)
	{		
		relay_system->MasterSocketActivity();
	}
}



/*
 * Portal Seekers Server Main 
*/

int main(int argc, char *argv[])
{
	//Matchmaking Setup
	if (argc < 3) {
		fprintf(stderr, "ERROR, provide PORT and NUMBER_OF_PLAYERS per game\n");
		exit(1);
	}

	Room_Manager* room_manager = new Room_Manager();

	
	//Initialize matchmaking
	matchmaking = new Matchmaking(room_manager);
	matchmaking->SetPort(atoi(argv[1]));
	matchmaking->SetPlayersPerGame(atoi(argv[2]));
	matchmaking->Initialize();
	//RunMatchmaking();	
	std::thread matchmaking_thread(RunMatchmaking);
	std::cout << "Matchmaking Thread is running" << std::endl;
	
	//Initialize relay system
	relay_system = new RelaySystem(room_manager);
	relay_system->SetPort(atoi(argv[1]));	
	relay_system->Initialize();
	//RunRelaySystem();
	std::thread relay_thread(RunRelaySystem);
	std::cout << "Relay thread is running" << std::endl;
		
	
	while (true) {
		std::string command;		
		std::getline(std::cin, command);
		
		//Split String into array
		std::string cm_array[2];
		int i = 0;
		std::stringstream stream(command);
		while (stream.good() && i < 2) {
			stream >> cm_array[i];
			++i;
		}

		//Displays information about all the rooms available
		if (command.compare("show rooms") == 0 || command.compare("shroom") == 0) {
			room_manager->ShowRooms();
		}
		//Command to start current game
		else if(command.compare("start") == 0 || command.compare("sgame") == 0){
			matchmaking->StartGame();
		}	
		//Command to show all the players currently connected 
		else if (command.compare("shplayers") == 0 || command.compare("show players") == 0) {
			room_manager->ShowPlayers();
		}
		//Broadcast a message to all the players
		else if(cm_array[0].compare("bd") == 0){
			command.erase(0, 2);
			const char* message = command.c_str();
			char net_message [256];
			strcat(net_message, "4:");
			strcat(net_message, message);
			strcat(net_message, ";\r\n");
			matchmaking->BroadcastMessage(net_message);
			printf("Message Broadcast: %s \n", message);
		}
		else {
			std::cout << "Command invalid" << std::endl;
		}
		
		//show server statictis
		

	}	
	
	return 0;
}