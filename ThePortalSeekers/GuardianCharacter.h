// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThePortalSeekersCharacter.h"
//#include "Camera/CameraComponent.h"
#include "GuardianCharacter.generated.h"


/**
 * Guardian Character Class
 * Blueprint inherits from this class
 */
UCLASS()
class THEPORTALSEEKERS_API AGuardianCharacter : public AThePortalSeekersCharacter
{
	GENERATED_BODY()
	
public:
	// Sets default values
	AGuardianCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UPROPERTY(EditAnywhere)
		USceneComponent* OurVisibleComponent;
		//USpringArmComponent* OurCameraSpringArm;
		//UCameraComponent* OurCamera;
};
