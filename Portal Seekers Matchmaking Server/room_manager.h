#pragma once
#include "room.h"
/*
* Class that handles all the room management
* Responsible to create and delete rooms
*/

class Room_Manager
{
public:
	Room_Manager();
	~Room_Manager();
	
	//Closes a room where client with _socket is
	void CloseRoom(int _socket);
	void CloseRoom(Room* room);
	//Getter for the Current Room
	Room* GetCurrentRoom();
	//Save the user address to the current room
	void AddNewConnection(sockaddr_in _addreess, int socket);
	//Searches for the player witn @address and saves the inner local network address @inner_address and port
	void SavePlayerAddress(sockaddr_in address, sockaddr_in inner_address);
	//Informs the room manager to starte the game
	void StartGame();
	//Setter for teh current room
	void SetCurrentRoom(Room* _room);
	//Checks if @_room is the current active room 
	bool IsCurrentRoom(Room* _room);
	//Get the all the client sockets that share the room with the client to which _socket belongs to
	int* GetAllSocketsOfRoomBySocket(int _socket);
	//Get the vector containing all the rooms by the address of one of the clients
	Room* GetRoomByAddress(sockaddr_in _addreess);
	//Get the vector containing all the rooms by the socket of one of the clients
	Room* GetRoomBySocket(int socket);
	//Getter Method for m_rooms
	std::vector<Room*> GetRooms();
	//Prints the number of rooms and the players in each room
	void ShowRooms();
	//Prints all the information of all connected players 
	void ShowPlayers();

private:
	//Creates a new room
	Room* CreateRoom();
	//Vector of all the active rooms
	std::vector<Room*> m_rooms;
	//Current room that is being filled with players
	Room* m_currentRoom = nullptr;
};



