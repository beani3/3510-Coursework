#include "PhysicsObstacles.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

APhysicsObstacles::APhysicsObstacles()
{
	if (Mesh) // Enable physics simulation on the mesh
	{
		Mesh->SetSimulatePhysics(true);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
		Mesh->SetMobility(EComponentMobility::Movable);
	}

	if (Collision)// Set collision profile and enable hit events
	{
		Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
		Collision->SetNotifyRigidBodyCollision(true);
	}
}

void APhysicsObstacles::HandlePostEffectsOnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, const FVector& NormalImpulse, const FHitResult& Hit) //handles effects after hit
{
	Super::HandlePostEffectsOnHit(OtherActor, OtherComp, NormalImpulse, Hit);

	if (Mesh && Mesh->IsSimulatingPhysics()) // Apply scaled impulse to the mesh at the hit location
	{
		const FVector AppliedImpulse = NormalImpulse * ImpulseScale;
		Mesh->AddImpulseAtLocation(AppliedImpulse, Hit.ImpactPoint, NAME_None);
	}
}