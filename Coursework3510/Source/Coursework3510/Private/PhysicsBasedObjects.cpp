// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicsBasedObjects.h"

APhysicsBasedObjects::APhysicsBasedObjects()
{

}

void APhysicsBasedObjects::BeginPlay()
{
	Super::BeginPlay();
	
}

void APhysicsBasedObjects::HandleVehicleHit(AActor* OtherActor, const FHitResult& Hit, float OtherSpeed)
{
	return;
}