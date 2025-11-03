// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	// Collision
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(16.f);
	Collision->SetCollisionProfileName(TEXT("BlockAllDynamic")); // blocks world + pawns
	Collision->SetGenerateOverlapEvents(false);
	SetRootComponent(Collision);

	// Mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));	
	Mesh->SetupAttachment(Collision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Movement
	Move = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Move"));
	Move->SetUpdatedComponent(Collision);
	Move->bRotationFollowsVelocity = true;
	Move->ProjectileGravityScale = 0.f;
	Move->bInitialVelocityInLocalSpace = false;
	
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	
	Collision->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);// register hit event
	Move->OnProjectileBounce.AddDynamic(this, &AProjectile::OnBounce);// register bounce event

	if (Data && Data->LifeSeconds > 0.f)// set lifespan
		SetLifeSpan(Data->LifeSeconds);
}

// Initialize from definition
void AProjectile::InitFromDef(const UProjectileDef* Def, AActor* InInstigator, USceneComponent* HomingTarget)
{
	Data = Def; InstigatorActor = InInstigator; check(Data);

	if (UStaticMesh* M = Data->Mesh.LoadSynchronous())         Mesh->SetStaticMesh(M); // set mesh
	if (UMaterialInterface* Mat = Data->MeshMaterial.LoadSynchronous()) Mesh->SetMaterial(0, Mat);// set material

	if (InstigatorActor) Collision->IgnoreActorWhenMoving(InstigatorActor, true);// ignore instigator for collision

	// get shoot direction
	const FVector ShootDir = GetActorTransform().GetRotation().GetForwardVector().GetSafeNormal();

	// configure movement
	Move->InitialSpeed = Data->Speed; // initial speed
	Move->MaxSpeed = Data->Speed; // max speed
	Move->bInitialVelocityInLocalSpace = false;          // velocity is in world-space
	Move->Velocity = ShootDir * Data->Speed; // set velocity
	Move->bRotationFollowsVelocity = true; //rotate to face direction

	Move->bShouldBounce = (Data->Behavior == EProjBehavior::Bouncy);
	Move->Bounciness = 0.5f;
	Move->Friction = 0.2f;

	const bool bHoming = (Data->Behavior == EProjBehavior::Homing);
	Move->bIsHomingProjectile = bHoming;
	if (bHoming && HomingTarget)
	{
		Move->HomingTargetComponent = HomingTarget;
		Move->HomingAccelerationMagnitude = 8000.f;
	}

	// Play fire sound ( not used yet)
	if (!Data->FireSFX.IsNull())
		UGameplayStatics::PlaySoundAtLocation(this, Data->FireSFX.LoadSynchronous(), GetActorLocation());
}

bool AProjectile::ShouldBounceOff(const AActor* Other) const
{
	if (!Other) return true;
	for (const FName& N : Data->NoBounceTags) if (Other->ActorHasTag(N)) return false;
	if (Data->AllowedBounceTags.Num() == 0) return true;
	for (const FName& A : Data->AllowedBounceTags) if (Other->ActorHasTag(A)) return true;
	return false;
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* Other, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!Move->bShouldBounce || !ShouldBounceOff(Other))
	{
		// Play impact sound (not used yet)
		if (!Data->ImpactSFX.IsNull())
			UGameplayStatics::PlaySoundAtLocation(this, Data->ImpactSFX.LoadSynchronous(), Hit.ImpactPoint);
		// Destroy projectile
		Die();
	}
}

void AProjectile::OnBounce(const FHitResult& Impact, const FVector& Vel)
{
	if (++BounceCount >= Data->MaxBounces)
	{
		// Play impact sound (not used yet)
		if (!Data->ImpactSFX.IsNull())
			UGameplayStatics::PlaySoundAtLocation(this, Data->ImpactSFX.LoadSynchronous(), Impact.ImpactPoint);
		// Destroy projectile
		Die();
	}
}

void AProjectile::Die() // destroy projectile
{
	Destroy();
}