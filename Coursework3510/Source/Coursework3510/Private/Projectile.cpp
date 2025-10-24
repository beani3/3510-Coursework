// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);
	Mesh->SetGenerateOverlapEvents(false);

	Move = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Move"));
	Move->bRotationFollowsVelocity = true;
	Move->ProjectileGravityScale = 0.f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	Mesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	Move->OnProjectileBounce.AddDynamic(this, &AProjectile::OnBounce);

	if (Data.LifeSeconds > 0.f)
		SetLifeSpan(Data.LifeSeconds);
}

void AProjectile::InitFromItemRow(const FProjectileRow& Row, AActor* InInstigator, USceneComponent* HomingTarget)
{
	Data = Row;
	InstigatorActor = InInstigator;

	if (UStaticMesh* M = Row.Mesh.LoadSynchronous())
		Mesh->SetStaticMesh(M);

	if (UMaterialInterface* Mat = Row.MeshMaterial.LoadSynchronous())
		Mesh->SetMaterial(0, Mat);

	Move->InitialSpeed = Row.Speed;
	Move->MaxSpeed = Row.Speed;
	Move->Velocity = GetActorForwardVector() * Row.Speed;

	const bool bBounce = (Row.Behavior == EProjBehavior::Bouncy || Row.Behavior == EProjBehavior::Homing);
	Move->bShouldBounce = (Row.Behavior == EProjBehavior::Bouncy);
	Move->Bounciness = 0.5f;
	Move->Friction = 0.2f;

	const bool bHoming = (Row.Behavior == EProjBehavior::Homing);
	Move->bIsHomingProjectile = bHoming;
	if (bHoming && HomingTarget)
	{
		Move->HomingTargetComponent = HomingTarget;
		Move->HomingAccelerationMagnitude = 8000.f;
	}

	//add effects later if needed
}

bool AProjectile::ShouldBounceOff(const AActor* Other) const
{
	if (!Other) return true;

	for (const FName& N : Data.NoBounceTags)
		if (Other->ActorHasTag(N))
			return false;

	if (Data.AllowedBounceTags.Num() == 0)
		return true;

	for (const FName& A : Data.AllowedBounceTags)
		if (Other->ActorHasTag(A))
			return true;

	return false;
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* Other, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!Move->bShouldBounce || !ShouldBounceOff(Other))
	{
		//add impact effects here later

		Die();
	}
}

void AProjectile::OnBounce(const FHitResult& Impact, const FVector& Vel)
{
	if (++BounceCount >= Data.MaxBounces)
	{
		//if (!Data.ImpactSFX.IsNull())
			//UGameplayStatics::PlaySoundAtLocation(this, Data.ImpactSFX.LoadSynchronous(), Impact.ImpactPoint);

		Die();
	}
}

void AProjectile::Die()
{
	Destroy();
}
// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

