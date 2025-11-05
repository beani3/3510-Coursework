#include "ObstaclesBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

AObstaclesBase::AObstaclesBase()
{
    PrimaryActorTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh")); //Mesh component set up
    RootComponent = Mesh;
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision")); //collision set up
    Collision->SetupAttachment(Mesh);
    Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    Collision->SetGenerateOverlapEvents(false);
    Collision->OnComponentHit.AddDynamic(this, &AObstaclesBase::OnCollisionHit);


	HealthComp = CreateDefaultSubobject<UAC_HealthComponent>(TEXT("HealthComp")); //health component set up
    HealthComp->SetComponentTickEnabled(true);
}

void AObstaclesBase::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
	ApplyData(); //apply data in editor
}

void AObstaclesBase::BeginPlay()
{
    Super::BeginPlay();

	if (HealthComp) //bind death event
    {
        HealthComp->OnDied.AddDynamic(this, &AObstaclesBase::OnDied_Handle);
    }
}

void AObstaclesBase::ApplyData()
{
	if (!Data) return; // No data to apply

    if (Data->Mesh)
    {
        Mesh->SetStaticMesh(Data->Mesh);
        for (int32 i = 0; i < Data->OverrideMaterials.Num(); ++i)
        {
            Mesh->SetMaterial(i, Data->OverrideMaterials[i]);
        }
    }

	Mesh->SetRelativeScale3D(Data->RelativeScale3D); //Transofrm overrides
    //Mesh->SetRelativeRotation(Data->RelativeRotation);


    Mesh->SetCollisionProfileName(Data->CollisionProfileName);
    Mesh->SetMobility(Data->Mobility);


	const bool bSim = Data->bSimulatePhysics && (Data->Mobility == EComponentMobility::Movable); // Determine if physics should be simulated
	Mesh->SetSimulatePhysics(bSim); // Physics simulation

	if (Data->PhysMatOverride) // Physical material override
        Mesh->SetPhysMaterialOverride(Data->PhysMatOverride);

	if (Data->bOverrideMass && bSim) // Mass override
        Mesh->SetMassOverrideInKg(NAME_None, FMath::Max(0.1f, Data->MassInKg), true);


	if (bAutoSizeCollisionToMeshBounds && Collision && Mesh->GetStaticMesh()) // Auto size collision box to mesh bounds
    {
        const FVector Extent = Mesh->Bounds.BoxExtent;
        Collision->SetBoxExtent(Extent);
        Collision->SetRelativeLocation(FVector::ZeroVector);
    }


	HitVFX = Data->HitVFX ? Data->HitVFX : HitVFX; // FX overrides
    HitSFX = Data->HitSFX ? Data->HitSFX : HitSFX;
    VFXScale = (Data->VFXScale > 0.f) ? Data->VFXScale : VFXScale;



	if (HealthComp) // Health configuration
    {
		if (Data->Health.bHasHealth) // Apply health config if obstacle has health
        {
            FHealthComponentConfig Cfg;
            Cfg.MaxHealth = Data->Health.MaxHealth;
            Cfg.bCanDie = Data->Health.bCanDie;
            Cfg.bAutoRegen = Data->Health.bAutoRegen;
            Cfg.RegenPerSecond = Data->Health.RegenPerSecond;
            Cfg.RegenDelay = Data->Health.RegenDelay;
            HealthComp->ApplyConfig(Cfg, true);
            HealthComp->SetComponentTickEnabled(true);
        }
        else
        {
            HealthComp->SetComponentTickEnabled(false);
        }
    }
}

void AObstaclesBase::OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    HandlePostEffectsOnHit(OtherActor, OtherComp, NormalImpulse, Hit);

	if (Data && Data->Health.bHasHealth && HealthComp) // Apply damage based on impact impulse
    {
        const float Damage = NormalImpulse.Size() * Data->Health.ImpactDamageScale;
        if (Damage > 0.f)
        {
            HealthComp->ApplyDamage(Damage);
        }
    }
}

void AObstaclesBase::HandlePostEffectsOnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, const FVector& NormalImpulse, const FHitResult& Hit)
{
	if (bFXPlayed) return; // Prevent multiple FX plays
    bFXPlayed = true;
    PlayEffectsAtHit(Hit);
}

void AObstaclesBase::PlayEffectsAtHit(const FHitResult& Hit) // Plays hit effects at the impact location
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

void AObstaclesBase::OnDied_Handle() // Handles obstacle death
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
