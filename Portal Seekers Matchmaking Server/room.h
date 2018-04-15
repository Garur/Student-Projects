#pragma once
#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <unistd.h>  
#include <arpa/inet.h>
#include "player.h"

/*
* Room class responsible to keep the information of a game being created or live game
*/
class Room
{
public:
	Room();
	~Room();
	bool isPlaying = false;
	int max_clients = 5;
	std::vector<Player*> players = std::vector<Player*>();
	//Adds a new player to the room and saves the @address and the port number
	void AddPlayer(sockaddr_in address, int _socket);
	//Searches for the player with @address and saves the inner local network address @inner_address and port
	void SavePlayerAddress(sockaddr_in address, sockaddr_in inner_address);
	//Returns the number of players currently in the room
	int GetNumberOfPlayers();
	//Getter for an array of sockets of all the player in the room
	int* GetPlayerSockets();

	void RemovePlayer(sockaddr_in _address);

	void PlayerDisconnected();

private:
	void RemovePlayer(Player* _player);
	Player* GetPlayerByAddress(sockaddr_in _address);
};

