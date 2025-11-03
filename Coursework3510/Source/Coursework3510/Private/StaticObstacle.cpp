#include "StaticObstacle.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

AStaticObstacle::AStaticObstacle()
{
	if (Mesh)
	{
		Mesh->SetSimulatePhysics(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
		Mesh->SetMobility(EComponentMobility::Static);
	}

	if (Collision)
	{
		Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
		Collision->SetGenerateOverlapEvents(false);
	}
}