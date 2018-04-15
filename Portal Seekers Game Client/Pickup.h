// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class THEPORTALSEEKERS_API APickup : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APickup();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	/** Return the mesh for the pickup */
	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return PickupMesh; }

	//Return wheter or not the pickup is active
	UFUNCTION(BlueprintPure, Category = "Pickup")
		bool IsActive();
	//Allows other class to safely change wheter or not the pickup is active
	UFUNCTION(BlueprintCallable, Category = "Pickup")
		void SetActive(bool NewPickupState);

	//Function to call when the pickup is collected
	UFUNCTION(BlueprintNativeEvent)
	void GetCollected();
	virtual void GetCollectedImplementation();


protected:
	/** True when the pickup is can be used and false when the pick up is deactivated*/
	bool bIsActive;


private:
	// Static Mesh to represent the pick up in the level
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* PickupMesh;
	
};
