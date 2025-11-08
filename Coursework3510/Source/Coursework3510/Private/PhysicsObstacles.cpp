#include "PhysicsObstacles.h"
#include "Components/StaticMeshComponent.h"

APhysicsObstacles::APhysicsObstacles()
{
	// Ensure the mesh simulates and blocks
	if (UStaticMeshComponent* M = FindComponentByClass<UStaticMeshComponent>())
	{
		M->SetSimulatePhysics(true);
		M->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		M->SetCollisionProfileName(TEXT("PhysicsActor"));
		M->SetMobility(EComponentMobility::Movable);
	}
}
