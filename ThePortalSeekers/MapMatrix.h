// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapMatrix.generated.h"

UCLASS()
class THEPORTALSEEKERS_API AMapMatrix : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapMatrix();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//Map dimensions
	int sizeX;
	int sizeY;
	void CreateMap();
	//Map definitions
	bool isWalkable;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
	
};
