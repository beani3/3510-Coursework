// ObstaclesBase.cpp (key changes only)
#include "ObstaclesBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

AObstaclesBase::AObstaclesBase()
{
    PrimaryActorTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
    Collision->SetupAttachment(Mesh);
    Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    Collision->SetGenerateOverlapEvents(false);
    Collision->OnComponentHit.AddDynamic(this, &AObstaclesBase::OnCollisionHit);

    //NEW: health subobject (always present; enabled by Data)
    HealthComp = CreateDefaultSubobject<UAC_HealthComponent>(TEXT("HealthComp"));
    HealthComp->SetComponentTickEnabled(true);
}

void AObstaclesBase::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    ApplyData(); //NEW
}

void AObstaclesBase::BeginPlay()
{
    Super::BeginPlay();
    ApplyData(); //ensure runtime matches editor

    if (HealthComp)
    {
        HealthComp->OnDied.AddDynamic(this, &AObstaclesBase::OnDied_Handle);
    }
}

void AObstaclesBase::ApplyData()
{
    if (!Data) return;

    // Mesh & materials
    if (Data->Mesh)
    {
        Mesh->SetStaticMesh(Data->Mesh);
        // Materials (optional)
        for (int32 i = 0; i < Data->OverrideMaterials.Num(); ++i)
        {
            Mesh->SetMaterial(i, Data->OverrideMaterials[i]);
        }
    }

    // Transform offsets
    Mesh->SetRelativeScale3D(Data->RelativeScale3D);
    Mesh->SetRelativeRotation(Data->RelativeRotation);

    // Collision / physics
    Mesh->SetCollisionProfileName(Data->CollisionProfileName);
    Mesh->SetMobility(Data->Mobility);

    const bool bSim = Data->bSimulatePhysics && (Data->Mobility == EComponentMobility::Movable);
    Mesh->SetSimulatePhysics(bSim);

    if (Data->PhysMatOverride)
        Mesh->SetPhysMaterialOverride(Data->PhysMatOverride);
    if (Data->bOverrideMass && bSim)
        Mesh->SetMassOverrideInKg(NAME_None, FMath::Max(0.1f, Data->MassInKg), true);

    // Auto-size collision to mesh bounds (same as your current logic)
    if (bAutoSizeCollisionToMeshBounds && Collision && Mesh->GetStaticMesh())
    {
        const FVector Extent = Mesh->Bounds.BoxExtent;
        Collision->SetBoxExtent(Extent);
        Collision->SetRelativeLocation(FVector::ZeroVector);
    }

    // FX overrides from Data (fallback to actor properties if Data fields are null)
    HitVFX = Data->HitVFX ? Data->HitVFX : HitVFX;
    HitSFX = Data->HitSFX ? Data->HitSFX : HitSFX;
    VFXScale = (Data->VFXScale > 0.f) ? Data->VFXScale : VFXScale;

    // HEALTH
    if (HealthComp)
    {
        if (Data->Health.bHasHealth)
        {
            FHealthComponentConfig Cfg;
            Cfg.MaxHealth = Data->Health.MaxHealth;
            Cfg.bCanDie = Data->Health.bCanDie;
            Cfg.bAutoRegen = Data->Health.bAutoRegen;
            Cfg.RegenPerSecond = Data->Health.RegenPerSecond;
            Cfg.RegenDelay = Data->Health.RegenDelay;
            HealthComp->ApplyConfig(Cfg, /*bResetHealth*/ true);
            HealthComp->SetComponentTickEnabled(true);
        }
        else
        {
            HealthComp->SetComponentTickEnabled(false);
        }
    }
}

void AObstaclesBase::OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    HandlePostEffectsOnHit(OtherActor, OtherComp, NormalImpulse, Hit);

    //NEW: convert impact into self-damage if this prop has health
    if (Data && Data->Health.bHasHealth && HealthComp)
    {
        const float Damage = NormalImpulse.Size() * Data->Health.ImpactDamageScale;
        if (Damage > 0.f)
        {
            HealthComp->ApplyDamage(Damage);
        }
    }
}

void AObstaclesBase::HandlePostEffectsOnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp,
    const FVector& NormalImpulse, const FHitResult& Hit)
{
    if (bFXPlayed) return; // your existing one-shot guard
    bFXPlayed = true;
    PlayEffectsAtHit(Hit);
}

void AObstaclesBase::PlayEffectsAtHit(const FHitResult& Hit)
{
    UWorld* World = GetWorld();
    if (!World) return;

    if (HitVFX)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, HitVFX, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), FVector(VFXScale));
    }
    if (HitSFX)
    {
        UGameplayStatics::PlaySoundAtLocation(this, HitSFX, Hit.ImpactPoint);
    }
}

void AObstaclesBase::OnDied_Handle()
{
    if (!Data) { Destroy(); return; }

    if (Data->bDestroyOnDeath)
    {
        Destroy();
    }
    else if (Data->BrokenMesh && Mesh)
    {
        Mesh->SetStaticMesh(Data->BrokenMesh);
        Mesh->SetSimulatePhysics(true);
    }
}
