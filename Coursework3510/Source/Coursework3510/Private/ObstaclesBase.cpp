// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstaclesBase.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/BodyInstance.h"

// Sets default values
AObstaclesBase::AObstaclesBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObstacleMesh"));
	SetRootComponent(Mesh);

	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetNotifyRigidBodyCollision(true); //enable hit events

	bReplicates = true;
	SetReplicateMovement(false); //the inherited classes will handle movement replication if neceesary.

	ImpactVFXMinSpeed = 600.f;
	CooldownBetweenVFX = 0.5f;
	DamageToVehicle = 10.f;
	bAffectVehicleOnHit = true;
	LastVFXTime = -CooldownBetweenVFX; //initialize to allow immediate VFX on first hit.

}

// Called when the game starts or when spawned
void AObstaclesBase::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(Mesh))
	{
		Mesh->OnComponentHit.AddDynamic(this, &AObstaclesBase::OnMeshHit);
	}
	
}

// Called every frame
void AObstaclesBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AObstaclesBase::OnMeshHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!IsAuth() || !OtherActor || OtherActor == this)
	{
		return; //only handle on server and valid actors.
	}

	const float OtherSpeed = GetVehicleSpeed(OtherActor); 

	HandleVehicleHit(OtherActor, Hit, OtherSpeed);

	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	if (OtherSpeed >= ImpactVFXMinSpeed && (Now - LastVFXTime) >= CooldownBetweenVFX)
	{
		PlayHitEffects(OtherSpeed, Hit.ImpactPoint, Hit.ImpactNormal);
		LastVFXTime = Now;
	}
}

void AObstaclesBase::HandleVehicleHit(AActor* OtherActor, const FHitResult& Hit, float OtherSpeed)
{
    // Base implementation does nothing
}

float AObstaclesBase::GetVehicleSpeed(AActor* Actor)
{
    if (!Actor)
	{
		return 0.f;
	}
	const FVector Velocity = Actor->GetVelocity();
	return Velocity.Size();
}

