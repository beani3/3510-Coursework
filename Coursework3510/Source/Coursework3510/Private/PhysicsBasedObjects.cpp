// Fill out your copyright notice in the Description page of Project Settings.

#include "PhysicsBasedObjects.h"

APhysicsBasedObjects::APhysicsBasedObjects()
{
	SetReplicateMovement(true);

	ImpulseScale = 1.0f;
	MaxImpulse = 150000.f;
	MaxLinearSpeed = 3000.f;
	bUseAddImpulseAtLocation = true;
	AssumedVehicleMassKg = 1000.f;
	bEnableCCD = true;

	if (Mesh)
	{
		Mesh->SetMobility(EComponentMobility::Movable);
		Mesh->SetSimulatePhysics(true);
		Mesh->BodyInstance.bUseCCD = bEnableCCD;
		Mesh->SetEnableGravity(true);
	}
}

void APhysicsBasedObjects::BeginPlay()
{
	Super::BeginPlay();

	if (Mesh)
	{
		Mesh->SetSimulatePhysics(true);
		Mesh->BodyInstance.bUseCCD = bEnableCCD;
	}
}

void APhysicsBasedObjects::HandleVehicleHit(AActor* OtherActor, const FHitResult& Hit, float OtherSpeed)
{
	if (!Mesh || !Mesh->IsSimulatingPhysics() || !IsAuth())
	{
		return;
	}

	Super::HandleVehicleHit(OtherActor, Hit, OtherSpeed);

	FVector OtherVel = OtherActor ? OtherActor->GetVelocity() : FVector::ZeroVector;
	const float Speed = OtherSpeed;
	FVector Dir = (OtherVel.Size() > KINDA_SMALL_NUMBER) ? OtherVel.GetSafeNormal() : (-Hit.ImpactNormal);

	const float MassKg = FMath::Max(AssumedVehicleMassKg, 100.f);
	FVector Impulse = Dir * (MassKg * Speed) * ImpulseScale;


	if (MaxImpulse > 0.f && Impulse.Size() > MaxImpulse)
	{
		Impulse = Impulse.GetClampedToMaxSize(MaxImpulse);
	}

	if (bUseAddImpulseAtLocation)
	{
		Mesh->AddImpulseAtLocation(Impulse, Hit.ImpactPoint, NAME_None);
	}
	else
	{
		Mesh->AddImpulse(Impulse, NAME_None, true);
	}

	if (MaxLinearSpeed > 0.f)
	{
		const FVector CurrentV = Mesh->GetPhysicsLinearVelocity();
		const float CurrentSpeed = CurrentV.Size();
		if (CurrentSpeed > MaxLinearSpeed)
		{
			const FVector Capped = CurrentV.GetSafeNormal() * MaxLinearSpeed;
			Mesh->SetPhysicsLinearVelocity(Capped, false, NAME_None);
		}
	}
}