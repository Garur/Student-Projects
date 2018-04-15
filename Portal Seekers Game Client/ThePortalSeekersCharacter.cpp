// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ThePortalSeekersCharacter.h"
#include "Network/MultiplayerActorTracker.h"


//////////////////////////////////////////////////////////////////////////
// AThePortalSeekersCharacter

AThePortalSeekersCharacter::AThePortalSeekersCharacter()
{
	
	//Create Multiplayer Actor Tracker
	MultiplayerActorTracker = CreateDefaultSubobject<UMultiplayerActorTracker>(TEXT("MultiplayerTracker"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)


	
}

void AThePortalSeekersCharacter::SetAIController()
{	
	//Set AI Controller	
	AIControllerClass = AAIController::StaticClass();
	SpawnDefaultController();
	MyAIControler = Cast<AAIController>(Controller);
}

AAIController * AThePortalSeekersCharacter::GetAIController()
{
	return MyAIControler;
}

bool AThePortalSeekersCharacter::HasGameStarted()
{
	AThePortalSeekersGameMode* gameMode = (AThePortalSeekersGameMode*)GetWorld()->GetAuthGameMode();
	return gameMode->HasGameStarted();
}

