
// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSynchronization.h"

GameSynchronization::GameSynchronization(UWorld* _World, int _localID, int _numOfPlayers) : m_World(_World), m_localID(_localID)
{
	//Initializes Variables
	m_currentClientTime = FDateTime();	
	m_ClockValues = new int[_numOfPlayers];
	for (int i = 0; i < _numOfPlayers; i++) {
		m_ClockValues[i] = 0;
	}
}

GameSynchronization::~GameSynchronization()
{

}

void GameSynchronization::Tick()
{	
	//Update current time
	FTimespan modifyTime = modifyTime.FromSeconds(FApp::GetDeltaTime());
	m_currentClientTime += modifyTime;
	//Update the vector clock every tick
	IncrementLocalVectorClock();
}

void GameSynchronization::IncrementLocalVectorClock()
{
	m_ClockValues[m_localID]++;	
}

int GameSynchronization::GetLocalVectorClock()
{
	return m_ClockValues[m_localID];
}

bool GameSynchronization::IncrementVectorClockValue(int playerID, int _receivedValue)
{
	//If value received is greater then the current value
	if (m_ClockValues[playerID] < _receivedValue) {
		//update value
		m_ClockValues[playerID] = _receivedValue;
		return true;
	}
	//else do not update value 
	return false;
	
}

FDateTime GameSynchronization::GetCurrentClientTime()
{
	return m_currentClientTime;
}

float GameSynchronization::CheckTimeDifference(FString _peerTimeStamp)
{
	FDateTime peeerTimeStamp = FDateTime();
	FDateTime::Parse(_peerTimeStamp, peeerTimeStamp);	
	//Subtracts the received peer time stamp with the timestamp belonging to this player
	FTimespan interval = m_currentClientTime - peeerTimeStamp;
	UE_LOG(LogTemp, Warning, TEXT("Time Difference is: %f"), interval.GetTotalMilliseconds());
	//returns the difference in milliseconds
	return interval.GetTotalMilliseconds();
}
