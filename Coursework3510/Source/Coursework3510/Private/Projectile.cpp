// Projectile.cpp

#include "Projectile.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AC_HealthComponent.h"
#include "AC_PointsComponent.h"
#include "Engine/World.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false); // enable only when needed (e.g., spline clamp / homing)

	bReplicates = true;
	SetReplicateMovement(true);

	Tags.Add(FName("Projectile"));

	// ==== COLLISION (hit-only) ====
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(16.f);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Collision->SetCollisionObjectType(ProjectileChannel);

	// Start ignore all, then explicitly enable what we want
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);

	// We want to hit world + pawns/vehicles
	Collision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	Collision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	Collision->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Block);

	// Projectiles ignore each other
	Collision->SetCollisionResponseToChannel(ProjectileChannel, ECR_Ignore);

	Collision->SetGenerateOverlapEvents(false);   // we rely on OnHit only
	Collision->SetNotifyRigidBodyCollision(true); // required for OnComponentHit

	SetRootComponent(Collision);

	// ==== MESH ====
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// ==== MOVEMENT ====
	Move = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Move"));
	Move->SetUpdatedComponent(Collision);
	Move->ProjectileGravityScale = 0.f;
	Move->bRotationFollowsVelocity = true;
	Move->bInitialVelocityInLocalSpace = false;

	// init pointers
	Data = nullptr;
	InstigatorActor = nullptr;
	RaceSpline = nullptr;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (!InstigatorActor)
		InstigatorActor = GetInstigator();

	// bind hit event
	Collision->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

void AProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bClampToSplineHeight)
	{
		ClampHeightToSpline(DeltaSeconds);
	}
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

USplineComponent* AProjectile::FindRaceSpline() const
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	// First try tag "Racetrack"
	{
		TArray<AActor*> Tagged;
		UGameplayStatics::GetAllActorsWithTag(World, FName("Racetrack"), Tagged);
		for (AActor* A : Tagged)
		{
			if (A)
			{
				if (USplineComponent* SC = A->FindComponentByClass<USplineComponent>())
				{
					return SC;
				}
			}
		}
	}

	// Fallback: any spline on actor named "Racetrack"
	{
		TArray<AActor*> All;
		UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), All);
		for (AActor* A : All)
		{
			if (!A) continue;

			if (A->GetName().Equals(TEXT("Racetrack"), ESearchCase::IgnoreCase))
			{
				if (USplineComponent* SC = A->FindComponentByClass<USplineComponent>())
				{
					return SC;
				}
			}
		}
	}

	return nullptr;
}

void AProjectile::InitFromDef(const UProjectileDef* Def, AActor* InInstigator, USceneComponent* HomingTarget)
{
	Data = Def;
	InstigatorActor = InInstigator;
	check(Data);

	if (APawn* P = Cast<APawn>(InInstigator))
		SetInstigator(P);
	SetOwner(InInstigator);

	// Shooter immunity (movement ignore + time window)
	if (InstigatorActor)
		Collision->IgnoreActorWhenMoving(InstigatorActor, true);

	IgnoreShooterUntilTime = GetWorld()
		? GetWorld()->GetTimeSeconds() + ArmingDelaySeconds
		: 0.0;

	ApplyVisualsFromDef(Data);
	ApplyLifespanFromDef(Data);

	// === Spline clamp ===
	RaceSpline = FindRaceSpline();
	bClampToSplineHeight = (RaceSpline != nullptr);
	if (bClampToSplineHeight)
	{
		SetActorTickEnabled(true);
	}

	// === Bounce vs Homing ===
	const bool bIsBouncy = (Data->Behavior == EProjBehavior::Bouncy);
	const bool bIsHoming = (Data->Behavior == EProjBehavior::Homing);

	Move->bShouldBounce = bIsBouncy && !bIsHoming; // homing  no bounce
	Move->Bounciness = bIsBouncy ? 0.5f : 0.0f;
	Move->Friction = 0.2f;

	if (bIsHoming && HomingTarget)
	{
		Move->bIsHomingProjectile = true;
		Move->HomingTargetComponent = HomingTarget;
		Move->HomingAccelerationMagnitude = HomingAccelerationMagnitude;

		// Aim directly at the target, with a slight height bias
		FVector TargetLoc = HomingTarget->GetComponentLocation();
		TargetLoc.Z += SplineHeightOffsetZ * 0.5f;

		const FVector DirToTarget = (TargetLoc - GetActorLocation()).GetSafeNormal();

		Move->InitialSpeed = Data->Speed;
		Move->MaxSpeed = Data->Speed;
		Move->Velocity = DirToTarget * Data->Speed;
	}
	else
	{
		Move->bIsHomingProjectile = false;
		Move->HomingTargetComponent = nullptr;

		const FVector ShootDir = GetActorQuat().GetForwardVector().GetSafeNormal();
		Move->InitialSpeed = Data->Speed;
		Move->MaxSpeed = Data->Speed;
		Move->Velocity = ShootDir * Data->Speed;
	}

	// Fire SFX
	if (!Data->FireSFX.IsNull())
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			Data->FireSFX.LoadSynchronous(),
			GetActorLocation()
		);
	}
}

void AProjectile::ClampHeightToSpline(float DeltaSeconds)
{
	if (!RaceSpline) return;

	FVector Loc = GetActorLocation();


	const float Key = RaceSpline->FindInputKeyClosestToWorldLocation(Loc);
	const FVector SplineLoc = RaceSpline->GetLocationAtSplineInputKey(Key, ESplineCoordinateSpace::World);

	const float DesiredZ = SplineLoc.Z + SplineHeightOffsetZ;

	Loc.Z = FMath::FInterpTo(Loc.Z, DesiredZ, DeltaSeconds, 10.f); 
	SetActorLocation(Loc, false, nullptr, ETeleportType::TeleportPhysics);
}

bool AProjectile::IsValidVictim(AActor* Other) const
{
	if (!Other || Other == InstigatorActor || Other == GetOwner())
		return false;

	const double Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
	if (Now < IgnoreShooterUntilTime)
		return false;

	if (Other->FindComponentByClass<UAC_HealthComponent>())
		return true;

	if (const UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Other->GetRootComponent()))
	{
		const ECollisionChannel Ch = Prim->GetCollisionObjectType();
		if (Ch == ECC_Pawn || Ch == ECC_Vehicle)
			return true;
	}

	return false;
}

void AProjectile::DoImpactOnValidVictim(AActor* Victim, const FVector& Where)
{
	if (!HasAuthority() || !Victim) return;

	if (UAC_HealthComponent* Health = Victim->FindComponentByClass<UAC_HealthComponent>())
	{
		Health->ApplyDamage(ImpactDamage);
	}

	if (InstigatorActor)
	{
		if (UAC_PointsComponent* Points = InstigatorActor->FindComponentByClass<UAC_PointsComponent>())
		{
			Points->AddPoints(ImpactPoints);
		}
	}

	if (Data && !Data->ImpactSFX.IsNull())
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			Data->ImpactSFX.LoadSynchronous(),
			Where
		);
	}

	Die();
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* Other, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!Data)
	{
		Die();
		return;
	}

	if (Other == InstigatorActor || Other == GetOwner())
		return;

	if (IsValidVictim(Other))
	{
		DoImpactOnValidVictim(Other, Hit.ImpactPoint);
		return;
	}

	const bool bIsHoming = (Data->Behavior == EProjBehavior::Homing);

	if (Move->bShouldBounce && !bIsHoming)
	{
		BounceCount++;
		if (BounceCount >= Data->MaxBounces)
		{
			if (Data && !Data->ImpactSFX.IsNull())
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					Data->ImpactSFX.LoadSynchronous(),
					Hit.ImpactPoint
				);
			}
			Die();
		}
	}
	else
	{
		if (Data && !Data->ImpactSFX.IsNull())
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				Data->ImpactSFX.LoadSynchronous(),
				Hit.ImpactPoint
			);
		}
		Die();
	}
}

void AProjectile::Die()
{
	Destroy();
}
