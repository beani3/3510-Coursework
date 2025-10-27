// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root collision (UpdatedComponent must be a collider)
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(16.f);
	Collision->SetCollisionProfileName(TEXT("BlockAllDynamic")); // blocks world + pawns
	Collision->SetGenerateOverlapEvents(false);
	SetRootComponent(Collision);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));	
	Mesh->SetupAttachment(Collision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	Move = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Move"));
	Move->SetUpdatedComponent(Collision);
	Move->bRotationFollowsVelocity = true;
	Move->ProjectileGravityScale = 0.f;
	
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	Collision->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	Move->OnProjectileBounce.AddDynamic(this, &AProjectile::OnBounce);

	if (Data && Data->LifeSeconds > 0.f)
		SetLifeSpan(Data->LifeSeconds);
}

void AProjectile::InitFromDef(const UProjectileDef* Def, AActor* InInstigator, USceneComponent* HomingTarget)
{
	Data = Def;
	InstigatorActor = InInstigator;
	check(Data);

	if (UStaticMesh* M = Data->Mesh.LoadSynchronous())     Mesh->SetStaticMesh(M);
	if (UMaterialInterface* Mat = Data->MeshMaterial.LoadSynchronous()) Mesh->SetMaterial(0, Mat);

	Move->InitialSpeed = Data->Speed;
	Move->MaxSpeed = Data->Speed;
	Move->Velocity = GetActorForwardVector() * Data->Speed;

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
		if (!Data->ImpactSFX.IsNull())
			UGameplayStatics::PlaySoundAtLocation(this, Data->ImpactSFX.LoadSynchronous(), Hit.ImpactPoint);
		Die();
	}
}

void AProjectile::OnBounce(const FHitResult& Impact, const FVector& Vel)
{
	if (++BounceCount >= Data->MaxBounces)
	{
		if (!Data->ImpactSFX.IsNull())
			UGameplayStatics::PlaySoundAtLocation(this, Data->ImpactSFX.LoadSynchronous(), Impact.ImpactPoint);
		Die();
	}
}

void AProjectile::Die()
{
	Destroy();
}