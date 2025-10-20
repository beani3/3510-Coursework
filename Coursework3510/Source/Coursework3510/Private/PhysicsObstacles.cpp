// Fill out your copyright notice in the Description page of Project Settings.

#include "PhysicsObstacles.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

APhysicsObstacles::APhysicsObstacles()
{
	// Enable physics simulation on the mesh so it can respond to impulses
	if (Mesh)
	{
		Mesh->SetSimulatePhysics(true);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
		Mesh->SetMobility(EComponentMobility::Movable);
	}

	// Collision box should block and generate hit events
	if (Collision)
	{
		Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
		Collision->SetNotifyRigidBodyCollision(true);
	}
}

void APhysicsObstacles::HandlePostEffectsOnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, const FVector& NormalImpulse, const FHitResult& Hit)
{
	// First run base behaviour (spawns FX/SFX and FX guard)
	Super::HandlePostEffectsOnHit(OtherActor, OtherComp, NormalImpulse, Hit);

	// Apply impulse to this obstacle's mesh if physics is enabled
	if (Mesh && Mesh->IsSimulatingPhysics())
	{
		// Convert collision normal impulse into an impulse applied at the hit location
		const FVector AppliedImpulse = NormalImpulse * ImpulseScale;
		Mesh->AddImpulseAtLocation(AppliedImpulse, Hit.ImpactPoint, NAME_None);
	}
}