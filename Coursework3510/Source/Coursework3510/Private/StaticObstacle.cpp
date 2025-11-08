#include "StaticObstacle.h"
#include "Components/StaticMeshComponent.h"

AStaticObstacle::AStaticObstacle()
{
	// Mesh blocks but doesn't simulate
	if (UStaticMeshComponent* M = FindComponentByClass<UStaticMeshComponent>())
	{
		M->SetSimulatePhysics(false);
		M->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		M->SetCollisionProfileName(TEXT("BlockAllDynamic")); // or your custom profile
		M->SetMobility(EComponentMobility::Static);
	}
}
