#include "room_manager.h"
#include <iostream>



Room_Manager::Room_Manager()
{
	m_rooms = std::vector<Room*>();
}


Room_Manager::~Room_Manager()
{

}



void Room_Manager::CloseRoom(int _socket) {
	Room* room = GetRoomBySocket(_socket);
	room = nullptr;
	delete room;
}

void Room_Manager::CloseRoom(Room* room) {	
	room = nullptr;
	delete room;
}

Room * Room_Manager::GetCurrentRoom()
{
	if (m_currentRoom == nullptr) {
		m_currentRoom = CreateRoom();
	}
	return m_currentRoom;
}

void Room_Manager::AddNewConnection(sockaddr_in _addreess, int _socket) {	
	
	if (m_currentRoom == nullptr) {
		m_currentRoom = CreateRoom();
	}
	//Save the new adress and add player to the current room
	m_currentRoom->AddPlayer(_addreess, _socket);
}

void Room_Manager::SavePlayerAddress(sockaddr_in main_address, sockaddr_in inner_address)
{
	GetCurrentRoom()->SavePlayerAddress(main_address, inner_address);
}

void Room_Manager::StartGame()
{
	GetCurrentRoom()->isPlaying = true;
	SetCurrentRoom(nullptr);
}


void Room_Manager::SetCurrentRoom(Room * _room)
{
	m_currentRoom = _room;
}

bool Room_Manager::IsCurrentRoom(Room * _room)
{
	return _room == m_currentRoom;
}

int* Room_Manager::GetAllSocketsOfRoomBySocket(int _socket)
{	
	return GetRoomBySocket(_socket)->GetPlayerSockets();
}

Room* Room_Manager::GetRoomByAddress(sockaddr_in _addreess)
{
	Room*  room; 
	Player* player;
	for (int i = 0; i < m_rooms.size(); i++) {
		room = m_rooms[i];
		for (int j = 0; j < room->players.size(); j++) {
			player = room->players[j];
			for (int k = 0; k < player->addresses.size(); k++) {
				//printf("Player ip is : %s , port : %d \n", inet_ntoa(player->addresses[k].sin_addr), ntohs(player->addresses[k].sin_port));
				//printf("Searched ip is : %s , port : %d \n", inet_ntoa(_addreess.sin_addr), ntohs(_addreess.sin_port));
				if (player->addresses[k].sin_addr.s_addr == _addreess.sin_addr.s_addr/* && player->addresses[k].sin_port == _addreess.sin_port*/) {
					//printf("Room found");
					return room;
				}
			}			
		}
	}
	//printf("Room not found");
	return nullptr;
}

Room* Room_Manager::GetRoomBySocket(int socket)
{
	for (int i = 0; i < m_rooms.size(); i++) {
		for (int j = 0; j < m_rooms[i]->max_clients; j++) {
			int* sockets = m_rooms[i]->GetPlayerSockets();
			if (sockets[j] == socket) {
				return m_rooms[i];
			}
		}
	}
	return nullptr;
}

std::vector<Room*> Room_Manager::GetRooms()
{
	return m_rooms;
}

Room * Room_Manager::CreateRoom()
{
	Room* room = new Room();
	room->isPlaying = false;
	m_rooms.push_back(room);
	m_currentRoom = room;
	return room;
}

void Room_Manager::ShowRooms()
{
	printf("Total rooms %d \n", m_rooms.size());
	for (int i = 0; i < m_rooms.size(); i++) {
		std::cout << "Room " << i << " has " << m_rooms[i]->GetNumberOfPlayers() << " players" << std::endl;
	}
}

void Room_Manager::ShowPlayers()
{
	int num_players = 0;	
	for (int i = 0; i < m_rooms.size(); i++) {
		for (int j = 0; j < m_rooms[i]->players.size(); j++) {
			num_players++;

		}
		
	}
	std::cout << "Total players connected: " << num_players << std::endl;
	for (int i = 0; i < m_rooms.size(); i++) {
		for (int j = 0; j < m_rooms[i]->players.size(); j++) {			
			printf("Rooom: %d, Player Address: %s:%d \n", i, inet_ntoa(m_rooms[i]->players[j]->main_address.sin_addr), ntohs(m_rooms[i]->players[j]->main_address.sin_port));
		}
	}
}






