// Fill out your copyright notice in the Description page of Project Settings.

#include "Seeker.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Pickup.h"
#include "Orb.h"
#include "Components/SphereComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Network/MultiplayerActorTracker.h"

ASeeker::ASeeker() {
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//Create Collection Sphere
	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	CollectionSphere->SetupAttachment(RootComponent);
	CollectionSphere->SetSphereRadius(200.0f);


	PickedUpOrb = nullptr;	
	FName socket = TEXT("spine_02"); //Skeletal Socket to attach the components
	//Create Point Light
	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, socket);
	PointLight->SetVisibility(false);
	//Create Light Mesh
	LightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightMesh"));
	LightMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, socket);
	LightMesh->SetVisibility(false);
	bHasOrb = false;
}

void ASeeker::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
	
	//Super::SetupPlayerInputComponent(PlayerInputComponent);	
	
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	

	PlayerInputComponent->BindAxis("MoveForward", this, &ASeeker::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASeeker::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASeeker::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASeeker::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ASeeker::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ASeeker::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ASeeker::OnResetVR);
	
	PlayerInputComponent->BindAction("Collect", IE_Pressed, this, &ASeeker::OrbInteraction);

}

void ASeeker::OrbInteraction() {
	
	if (!bHasOrb) {
		CollectPickups();
	}
	else if(bHasOrb) {
		Drop();
		//if has tracker and is being tracked
		UMultiplayerActorTracker* Tracker = GetMultiplayerActorTracker();
		if (Tracker->IsBeingTracked()) {
			//Register Orb Event
			Tracker->RegisterOrbPickup(false, this, PickedUpOrb);
		}
	}
}


void ASeeker::CollectPickups(){
	//Get all overlapping actors and store them in the array
	TArray<AActor*> CollectedActors;
	CollectionSphere->GetOverlappingActors(CollectedActors);
	//For each actor collected 
	for (int32 iCollected = 0; iCollected < CollectedActors.Num(); iCollected++) {
		//Cast the actor to a APickup
		APickup* const TestPickup = Cast<APickup>(CollectedActors[iCollected]);
		//If the cast is succeful and pickup is valid and active
		if (TestPickup && !TestPickup->IsPendingKill() && TestPickup->IsActive()) {
			//Call the pickup was collected function
			TestPickup->GetCollected();
			//Check to see if pickup is also battery
			AOrb* const TestOrb = Cast<AOrb>(TestPickup);
			if (TestOrb && !bHasOrb) {
				//Picks up orb	
				PointLight->SetVisibility(true);
				LightMesh->SetVisibility(true);
				bHasOrb = true; 
				PickedUpOrb = TestOrb;	
				//if player is being tracked
				UMultiplayerActorTracker* Tracker = GetMultiplayerActorTracker();
				if (Tracker->IsBeingTracked()) {
					//Register the orb pick up
					Tracker->RegisterOrbPickup(true, this, PickedUpOrb);
				}
			}				
			TestPickup->GetMesh()->SetVisibility(false);
			TestPickup->SetActive(false);
			break;// only picks up the first orb in the loop			
		}
	}
}

void ASeeker::Drop() {

	if (PickedUpOrb != nullptr) {
		UE_LOG(LogTemp, Log, TEXT("Dropped: %s"), *PickedUpOrb->GetName());
		PickedUpOrb->GetMesh()->SetVisibility(true);
		PickedUpOrb->SetActive(true);
		FVector ActorLocation = GetActorLocation(); //Get character current location
		ActorLocation.Z += 25;
		PickedUpOrb->SetActorLocation(ActorLocation); //Set the orb location the same as the character
		PointLight->SetVisibility(false);
		LightMesh->SetVisibility(false);
		bHasOrb = false;
	}
}

void ASeeker::Pick(AActor* Orb)
{
	//Cast the actor to a APickup
	APickup* const TestPickup = Cast<APickup>(Orb);
	//If the cast is succeful and pickup is valid and active
	if (TestPickup && !TestPickup->IsPendingKill() && TestPickup->IsActive()) {
		//Call the pickup was collected function
		TestPickup->GetCollected();
		//Check to see if pickup is also battery
		AOrb* const TestOrb = Cast<AOrb>(TestPickup);
		if (TestOrb || !bHasOrb) {
			//Picks up orb	
			PointLight->SetVisibility(true);
			LightMesh->SetVisibility(true);
			bHasOrb = true;
			PickedUpOrb = TestOrb;
			//PickedUpOrb
			UE_LOG(LogTemp, Log, TEXT(" Picked up: %s"), *PickedUpOrb->GetName());
		}

		TestPickup->GetMesh()->SetVisibility(false);
		TestPickup->SetActive(false);	
	}
	
}



void ASeeker::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ASeeker::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ASeeker::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ASeeker::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASeeker::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ASeeker::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ASeeker::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}	


