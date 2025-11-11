// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectile::AProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    // Replication
    bReplicates = true;
    SetReplicateMovement(true);

    // Collision
    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    Collision->InitSphereRadius(16.f);
    Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
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

    Collision->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
    Move->OnProjectileBounce.AddDynamic(this, &AProjectile::OnBounce);
}

void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AProjectile, DefPath);
}

void AProjectile::InitFromDef(const UProjectileDef* Def, AActor* InInstigator, USceneComponent* HomingTarget)
{
    Data = Def;
    InstigatorActor = InInstigator;
    check(Data);

    // NEW: replicate the soft path from inside the class
    DefPath = FSoftObjectPath(Def);

    ApplyVisualsFromDef(Data);
    ApplyLifespanFromDef(Data);

    if (InstigatorActor)
    {
        Collision->IgnoreActorWhenMoving(InstigatorActor, true);
    }

    const FVector ShootDir = GetActorTransform().GetRotation().GetForwardVector().GetSafeNormal();

    Move->InitialSpeed = Data->Speed;
    Move->MaxSpeed = Data->Speed;
    Move->bInitialVelocityInLocalSpace = false;
    Move->Velocity = ShootDir * Data->Speed;
    Move->bRotationFollowsVelocity = true;

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
    {
        UGameplayStatics::PlaySoundAtLocation(this, Data->FireSFX.LoadSynchronous(), GetActorLocation());
    }
}

void AProjectile::OnRep_DefPath()
{
    if (!Data && DefPath.IsValid())
    {
        if (const UProjectileDef* Loaded = Cast<UProjectileDef>(DefPath.TryLoad()))
        {
            Data = Loaded;
            ApplyVisualsFromDef(Data);
            ApplyLifespanFromDef(Data);
        }
    }
}

void AProjectile::ApplyVisualsFromDef(const UProjectileDef* Def)
{
    if (!Def) return;

    if (UStaticMesh* M = Def->Mesh.LoadSynchronous())
    {
        Mesh->SetStaticMesh(M);
    }
    if (UMaterialInterface* Mat = Def->MeshMaterial.LoadSynchronous())
    {
        Mesh->SetMaterial(0, Mat);
    }
}

void AProjectile::ApplyLifespanFromDef(const UProjectileDef* Def)
{
    if (Def && Def->LifeSeconds > 0.f)
    {
        SetLifeSpan(Def->LifeSeconds);
    }
}

bool AProjectile::ShouldBounceOff(const AActor* Other) const
{
    if (!Other || !Data) return true;

    for (const FName& N : Data->NoBounceTags)
        if (Other->ActorHasTag(N)) return false;

    if (Data->AllowedBounceTags.Num() == 0) return true;

    for (const FName& A : Data->AllowedBounceTags)
        if (Other->ActorHasTag(A)) return true;

    return false;
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* Other, UPrimitiveComponent* OtherComp,
    FVector NormalImpulse, const FHitResult& Hit)
{
    if (!Data) { Die(); return; }

    if (!Move->bShouldBounce || !ShouldBounceOff(Other))
    {
        if (!Data->ImpactSFX.IsNull())
        {
            UGameplayStatics::PlaySoundAtLocation(this, Data->ImpactSFX.LoadSynchronous(), Hit.ImpactPoint);
        }
        Die();
    }
}

void AProjectile::OnBounce(const FHitResult& Impact, const FVector& Vel)
{
    if (!Data) { Die(); return; }

    if (++BounceCount >= Data->MaxBounces)
    {
        if (!Data->ImpactSFX.IsNull())
        {
            UGameplayStatics::PlaySoundAtLocation(this, Data->ImpactSFX.LoadSynchronous(), Impact.ImpactPoint);
        }
        Die();
    }
}

void AProjectile::Die()
{
    Destroy();
}
