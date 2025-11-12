// Projectile.cpp
#include "Projectile.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "AC_HealthComponent.h"
#include "AC_PointsComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	// Collision
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(16.f);
	Collision->SetCollisionProfileName(TEXT("BlockAllDynamic")); // block world for bounces
	Collision->SetGenerateOverlapEvents(true);

	// Overlap players/vehicles so we don't physically stop them
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Collision->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Overlap);
	// If you use a custom vehicle channel, also:
	// Collision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

	SetRootComponent(Collision);

	// Mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Movement
	Move = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Move"));
	Move->SetUpdatedComponent(Collision);
	Move->ProjectileGravityScale = 0.f;
	Move->bRotationFollowsVelocity = true;
	Move->bInitialVelocityInLocalSpace = false;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Fallback if early overlaps happen before InitFromDef
	if (!InstigatorActor)
		InstigatorActor = GetInstigator();

	Collision->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnBeginOverlap);


}

void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProjectile, DefPath);
}

void AProjectile::SetDefPathFromDef(const UProjectileDef* Def)
{
	if (!HasAuthority() || !Def) return;

	const FSoftObjectPath PathFromObj(Def);
	if (PathFromObj.IsValid() && !PathFromObj.ToString().StartsWith(TEXT("/Engine/Transient")))
	{
		DefPath = PathFromObj;
	}
	else
	{
		DefPath.Reset();
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
		Mesh->SetStaticMesh(M);
	if (UMaterialInterface* Mat = Def->MeshMaterial.LoadSynchronous())
		Mesh->SetMaterial(0, Mat);
}

void AProjectile::ApplyLifespanFromDef(const UProjectileDef* Def)
{
	if (Def && Def->LifeSeconds > 0.f)
		SetLifeSpan(Def->LifeSeconds);
}

void AProjectile::InitFromDef(const UProjectileDef* Def, AActor* InInstigator, USceneComponent* HomingTarget)
{
	Data = Def;
	InstigatorActor = InInstigator;
	check(Data);

	if (APawn* P = Cast<APawn>(InInstigator))
		SetInstigator(P);
	SetOwner(InInstigator);

	// Shooter immunity: movement ignore + short arming delay
	if (InstigatorActor)
		Collision->IgnoreActorWhenMoving(InstigatorActor, true);

	IgnoreShooterUntilTime = GetWorld() ? GetWorld()->GetTimeSeconds() + ArmingDelaySeconds : 0.0;

	ApplyVisualsFromDef(Data);
	ApplyLifespanFromDef(Data);

	// Initial velocity
	const FVector ShootDir = GetActorQuat().GetForwardVector().GetSafeNormal();
	Move->InitialSpeed = Data->Speed;
	Move->MaxSpeed = Data->Speed;
	Move->Velocity = ShootDir * Data->Speed;
	Move->bRotationFollowsVelocity = true;
	Move->bInitialVelocityInLocalSpace = false;

	// Bounce config
	Move->bShouldBounce = (Data->Behavior == EProjBehavior::Bouncy);
	Move->Bounciness = 0.5f;
	Move->Friction = 0.2f;

	// Homing
	if (Data->Behavior == EProjBehavior::Homing && HomingTarget)
	{
		Move->bIsHomingProjectile = true;
		Move->HomingTargetComponent = HomingTarget;
		Move->HomingAccelerationMagnitude = 8000.f;
	}

	// Fire SFX
	if (!Data->FireSFX.IsNull())
		UGameplayStatics::PlaySoundAtLocation(this, Data->FireSFX.LoadSynchronous(), GetActorLocation());
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

bool AProjectile::IsValidVictim(AActor* Other) const
{
	if (!Other || Other == InstigatorActor || Other == GetOwner()) return false;

	const double Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
	if (Now < IgnoreShooterUntilTime) return false;

	// Health component  valid target
	if (Other->FindComponentByClass<UAC_HealthComponent>())
		return true;

	// Or root object type is Pawn/Vehicle
	if (const UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Other->GetRootComponent()))
	{
		const ECollisionChannel Ch = Prim->GetCollisionObjectType();
		if (Ch == ECC_Pawn || Ch == ECC_Vehicle)
			return true;
	}

	return false;
}

void AProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp,
	int32 BodyIndex, bool bFromSweep, const FHitResult& Sweep)
{
	if (!HasAuthority() || !Data || !Other || Other == InstigatorActor || Other == GetOwner())
		return;

	// Ignore self during arming period
	const double Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
	if (Now < IgnoreShooterUntilTime) return;

	// Homing projectiles hit via overlap
	if (Data->Behavior == EProjBehavior::Homing && IsValidVictim(Other))
	{
		FVector ImpactPoint = GetActorLocation();
		if (bFromSweep && Sweep.bBlockingHit)
			ImpactPoint = Sweep.ImpactPoint;

		DoImpactOnValidVictim(Other, ImpactPoint);
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* Other, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!Data) { Die(); return; }
	if (Other == InstigatorActor || Other == GetOwner()) return;

	// If we hit another player/car, deal damage and destroy
	if (IsValidVictim(Other))
	{
		DoImpactOnValidVictim(Other, Hit.ImpactPoint);
		return;
	}

	// Handle bouncy behavior
	if (Move->bShouldBounce && Data->Behavior == EProjBehavior::Bouncy)
	{
		BounceCount++;
		if (BounceCount >= Data->MaxBounces)
		{
			if (!Data->ImpactSFX.IsNull())
				UGameplayStatics::PlaySoundAtLocation(this, Data->ImpactSFX.LoadSynchronous(), Hit.ImpactPoint);
			Die();
		}
	}
	else
	{
		// Non-bouncy projectile dies immediately
		if (!Data->ImpactSFX.IsNull())
			UGameplayStatics::PlaySoundAtLocation(this, Data->ImpactSFX.LoadSynchronous(), Hit.ImpactPoint);
		Die();
	}
}

void AProjectile::DoImpactOnValidVictim(AActor* Victim, const FVector& Where)
{
	if (!Victim || Victim == InstigatorActor) return;

	// Apply damage
	if (UAC_HealthComponent* Health = Victim->FindComponentByClass<UAC_HealthComponent>())
	{
		Health->ApplyDamage(25.f);
	}

	// Give points to shooter
	if (UAC_PointsComponent* Points = InstigatorActor ? InstigatorActor->FindComponentByClass<UAC_PointsComponent>() : nullptr)
	{
		Points->AddPoints(50);
	}

	// Play sound
	if (!Data->ImpactSFX.IsNull())
	{
		UGameplayStatics::PlaySoundAtLocation(this, Data->ImpactSFX.LoadSynchronous(), Where);
	}

	// Optional: Add physical impulse
	if (UPrimitiveComponent* VictimRoot = Cast<UPrimitiveComponent>(Victim->GetRootComponent()))
	{
		if (VictimRoot->IsSimulatingPhysics())
		{
			VictimRoot->AddImpulseAtLocation(GetVelocity() * 50.f, Where);
		}
	}

	Die();
}


void AProjectile::Die()
{
	Destroy();
}
