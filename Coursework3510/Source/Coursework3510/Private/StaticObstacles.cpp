// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticObstacles.h"

AStaticObstacles::AStaticObstacles() 
{
	KnockbackScale = 100.0f;

	SetReplicateMovement(false);

	if (Mesh) 
	{
		Mesh->SetMobility(EComponentMobility::Static);
		Mesh->SetSimulatePhysics(false);
		Mesh->BodyInstance.bUseCCD = false;
		Mesh->SetEnableGravity(false);
	}
}

void AStaticObstacles::BeginPlay() 
{
	Super::BeginPlay();

	if (Mesh)
	{
		Mesh->SetSimulatePhysics(false);
	}
}

void AStaticObstacles::HandleVehicleHit(AActor* OtherActor, const FHitResult& Hit, float OtherSpeed) 
{
	if (!IsAuth())
	{
		return;
	}
}