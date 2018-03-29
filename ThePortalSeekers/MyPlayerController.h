// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ThePortalSeekersGameMode.h"
#include "MyPlayerController.generated.h"


/**
 * Responsible for controlling the player character
 */
UCLASS()
class THEPORTALSEEKERS_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public: 
	AMyPlayerController();
	~AMyPlayerController();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//Possess the player baseb on the Id
	void SetCurrentPlayer(int ID);
	//Returns the current Player Controlled Character
	AThePortalSeekersCharacter* GetCurrentPlayer();
	//Returns the player ID of the current Client
	int* GetCurrentPlayerID();
	
private:
	//Reference to the player character
	AThePortalSeekersCharacter* CurrentPlayer;
};
