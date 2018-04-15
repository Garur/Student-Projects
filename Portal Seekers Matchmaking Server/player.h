#pragma once
#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <unistd.h>  
#include <arpa/inet.h>

class Player
{
public:
	Player();
	~Player();
	sockaddr_in main_address;
	std::vector<sockaddr_in> addresses;	
	//socket associated with this player
	int socket;
	//ID of player in the current room and game
	int id;
};

