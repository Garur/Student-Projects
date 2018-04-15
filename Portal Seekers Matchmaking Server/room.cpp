#include "room.h"

Room::Room() {
	
}

Room::~Room()
{
}


void Room::AddPlayer(sockaddr_in _address, int _socket)
{	
	Player* new_player = new Player();
	//Set the main address
	new_player->main_address = _address;
	//Save the address of  the player
	new_player->addresses.push_back(new_player->main_address);
	//Set the socket corresponding to the player
	new_player->socket = _socket;
	// sets the id for the player in that room
	int playerId = players.size();
	new_player->id = playerId;
	//Add player to the vector of players in this room
	players.push_back(new_player);
		
}

void Room::SavePlayerAddress(sockaddr_in main_address, sockaddr_in address) {
	for (int i = 0; i < players.size(); i++) {	
		//Check which player has the main address in order to add it to the list 
		if (players[i]->main_address.sin_addr.s_addr == main_address.sin_addr.s_addr && players[i]->main_address.sin_port == main_address.sin_port) {
			for (int j = 0; j < players[i]->addresses.size(); j++) {
				//Check to see if this address as been added
				if (players[i]->addresses[j].sin_addr.s_addr == address.sin_addr.s_addr && players[i]->addresses[j].sin_port == address.sin_port) {
					break;
				}
				else {
					//Save new address 
					players[i]->addresses.push_back(address);
					break;
				}
			}		
		}
		else {
			//printf("Failled to store extra address of the player");
		}			
	}
}

int Room::GetNumberOfPlayers()
{
	return players.size();
}

int * Room::GetPlayerSockets()
{
	int* sockets_array = new int[players.size()];
	for (int i = 0; i < players.size(); i++){
		sockets_array[i] = players[i]->socket;
	}
	return sockets_array;
}

void Room::RemovePlayer(sockaddr_in _address)
{
	Player* player = GetPlayerByAddress(_address);
	if (player != nullptr) {
		RemovePlayer(player);
	}
	else {
		printf("Unable to remove player");
	}	
}

void Room::PlayerDisconnected()
{

}

void Room::RemovePlayer(Player * _player)
{
	int index = 0;
	for (int i = 0; i < players.size(); i++) {
		if (_player == players[i]) {
			index = i;
		}
	}
	delete(_player);
	_player = nullptr;
	players.erase(players.begin() + (index - 1));
	printf("Players size: %d", players.size());
}

Player* Room::GetPlayerByAddress(sockaddr_in _address)
{
	Player* player = nullptr;
	for (int i = 0; i < players.size(); i++) {
		player = players[i];
		if (player->main_address.sin_addr.s_addr == _address.sin_addr.s_addr && player->main_address.sin_port == _address.sin_port)
			return player;
		
		for (int j = 0; j < player->addresses.size(); j++) {
			if (player->addresses[j].sin_addr.s_addr == _address.sin_addr.s_addr && player->main_address.sin_port == _address.sin_port) {
				return player;
			}
		}
	}
	return nullptr;
}





