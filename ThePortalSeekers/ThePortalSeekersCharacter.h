// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "GameFramework/Character.h"
#include "ThePortalSeekersCharacter.generated.h"

/*
 * Parent class of guardian and seeker classes
 * Has the Player ID that is used to identify 
 * character and chose which char the player controls
 */

UCLASS(config=Game)
class AThePortalSeekersCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	AThePortalSeekersCharacter();

	//ID of this Player Character
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Multiplayer)		
	int32 PlayerId;
		
	void SetAIController();
	/** Returns the AI Controller **/
	AAIController* GetAIController();
	
	//States if the game has started and active
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	bool HasGameStarted();



protected:
	//Reference to the Network object tracker that registers event through the network
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Multiplayer, meta = (AllowPrivateAccess = "true"))
    class UMultiplayerActorTracker* MultiplayerActorTracker;
	//Reference to AI controller, only has value if character is not controlled by the player	
	AAIController* MyAIControler = nullptr;

public:
	
	/** Returns Multiplayer Tracker subobject **/
	FORCEINLINE class UMultiplayerActorTracker* GetMultiplayerActorTracker() const { return MultiplayerActorTracker; }
	
	

};

