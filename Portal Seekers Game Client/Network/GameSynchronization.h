// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Misc/App.h"
#include "Misc/DateTime.h"

/**
 * Responsible to store variables and methods that are used to implement game state synchronization
 * (Currently not fully implemented)
 */
class THEPORTALSEEKERS_API GameSynchronization
{
public:
	GameSynchronization(UWorld* _World, int localID, int _numOfPlayers);
	~GameSynchronization();
	//Update Clock Method
	void Tick();
	//Increments the local clock counter
	void IncrementLocalVectorClock();
	//Get value for the local vector clock of this player
	int GetLocalVectorClock();
	//Increments the current clock counter for a specific player. 
	//Returns true if value was updated. False if value of clock is less than the stored value, so it will not be updated
	bool IncrementVectorClockValue(int playerID, int _receivedValue);	
	//Returns the current time stamp on this client
	FDateTime GetCurrentClientTime();
	//Receives a peer timestamp and calculates the difference in miliseconds;
	float CheckTimeDifference(FString peerTimeStamp);
private:	
	//Pointer to the game world
	UWorld* m_World;
	//Current local time in milliseconds
	FDateTime m_currentClientTime;
	//Clock tick counter - one slot in the array for each peer	
	int* m_ClockValues;
	//Values that refer to how much desync peers are in relation to master clock
	int* m_OffSyncValues;
	//Local id of this player
	int m_localID;
};
