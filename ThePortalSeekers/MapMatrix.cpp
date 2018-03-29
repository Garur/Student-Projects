// Fill out your copyright notice in the Description page of Project Settings.

#include "MapMatrix.h"


// Sets default values
AMapMatrix::AMapMatrix()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMapMatrix::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMapMatrix::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//Spawn tiles, reference for movement and object placement 
void AMapMatrix::CreateMap()
{
	if (sizeX < sizeY) 
	{

	}
	if (sizeX > sizeY)
	{

	}

}
