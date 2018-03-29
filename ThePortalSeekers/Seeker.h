// out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ThePortalSeekersCharacter.h"
#include "Seeker.generated.h"

/**
 * Contains the logic necessary for the seekers class
 */
UCLASS()
class THEPORTALSEEKERS_API ASeeker : public AThePortalSeekersCharacter
{
	GENERATED_BODY()

public: 
	ASeeker();	
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	/** Collection Sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollectionSphere;
	
	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);
	
	//Called when press a key to collect any pickups inside the collection sphere
	
	UFUNCTION(BlueprintCallable, Category = "Pickups")
	void OrbInteraction();
	void CollectPickups();
	
	// Player Input
	//virtual void SetupPlayerInputComponent(UInputComponent * PlayerInputComponent) override;
	/** Point Light  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light", meta = (AllowPrivateAccess = "true"))
	class UPointLightComponent* PointLight;
	/** Light Mesh  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* LightMesh;
	
protected:
	// should i let this stay in ThePortalSeekersCharacter, or even copy to guardian
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);
	// End of APawn interface
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orb", meta = (AllowPrivateAccess = "true"))
	bool bHasOrb;
	class AOrb* PickedUpOrb;

public: 
	/** Returns CollectingSphere subobject **/
	FORCEINLINE class USphereComponent* GetCollectinSphere() const { return CollectionSphere; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns The Point Light subobject **/
	FORCEINLINE class UPointLightComponent* GetPointLight() const { return PointLight; }
	/** Returns The Light Mesh subobject **/
	FORCEINLINE class UStaticMeshComponent* GetLightMesh() const { return LightMesh; }
	
	void Pick(AActor* Orb);
	void Drop();

};
