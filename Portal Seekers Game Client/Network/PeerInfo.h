#pragma once

#include "CoreMinimal.h"

/**
 * Struct that contains all the information relative to a peer
 */
struct PeerInfo{
public:
	PeerInfo();
	//All the possible addresses of the peer
	TArray<FString> addresses;
	//Address test and working to perform comunnications
	FString chosenAddress = "\0";
	//Array containing all the ports for the addresses (index of array of addresses matchs the index of the ports)
	TArray<int> ports;
	//Port number corresponding to the choosen address
	int choosenPort;
	//Id of the peer
	int id;
	//States if this peer has confirmed that he is ready to play
	bool ready = false;

};
