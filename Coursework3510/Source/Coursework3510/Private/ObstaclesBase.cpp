#include "ObstaclesBase.h"
#include "ObstacleData.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "PhysicsEngine/BodyInstance.h"
#include "AC_HealthComponent.h"

// Scoring
#include "BPI_ScoreReceiver.h"      // interface
#include "AC_PointsComponent.h"     // component fallback

AObstaclesBase::AObstaclesBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// Mesh = root and only physics body
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetMobility(EComponentMobility::Movable);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	Mesh->SetSimulatePhysics(true);
	Mesh->SetNotifyRigidBodyCollision(true);
	Mesh->BodyInstance.bUpdateMassWhenScaleChanges = false;
	Mesh->OnComponentHit.AddDynamic(this, &AObstaclesBase::OnMeshHit);

	// Health component
	HealthComp = CreateDefaultSubobject<UAC_HealthComponent>(TEXT("HealthComp"));
	
}

void AObstaclesBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ApplyData(/*bApplyTransformOffsets*/ bApplyDAVisualOffsetsInConstruction);
}

void AObstaclesBase::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComp)
	{
		HealthComp->OnHealthChanged.AddDynamic(this, &AObstaclesBase::OnHealthChanged);
		HealthComp->OnDied.AddDynamic(this, &AObstaclesBase::OnDied_FromHealth);
	}

	// Runtime: do NOT override editor transforms
	ApplyData(/*bApplyTransformOffsets*/ false);
}

void AObstaclesBase::ApplyData(bool bApplyTransformOffsets)
{
	if (!Data || !Mesh) return;

	// Visuals
	if (Data->Mesh)
	{
		Mesh->SetStaticMesh(Data->Mesh);
	}
	for (int32 i = 0; i < Data->OverrideMaterials.Num(); ++i)
	{
		if (UMaterialInterface* Mat = Data->OverrideMaterials[i])
			Mesh->SetMaterial(i, Mat);
	}

	if (bApplyTransformOffsets)
	{
		Mesh->SetRelativeScale3D(Data->RelativeScale3D);
		Mesh->SetRelativeRotation(Data->RelativeRotation);
	}

	// Physics (on Mesh only)
	Mesh->SetCollisionProfileName(Data->CollisionProfileName);
	Mesh->SetMobility(Data->Mobility);
	Mesh->SetSimulatePhysics(Data->bSimulatePhysics);

	if (Data->bOverrideMass && Data->bSimulatePhysics)
	{
		Mesh->BodyInstance.bOverrideMass = true;
		Mesh->SetMassOverrideInKg(NAME_None, FMath::Max(0.001f, Data->MassInKg), true);
	}
	else
	{
		Mesh->BodyInstance.bOverrideMass = false;
		Mesh->SetMassOverrideInKg(NAME_None, 0.f, false);
	}

	// Health config
	if (HealthComp && Data->Health.bHasHealth)
	{
		FHealthComponentConfig Config;
		Config.MaxHealth = Data->Health.MaxHealth;
		Config.bCanDie = Data->Health.bCanDie;
		Config.bAutoRegen = Data->Health.bAutoRegen;
		Config.RegenPerSecond = Data->Health.RegenPerSecond;
		Config.RegenDelay = Data->Health.RegenDelay;

		HealthComp->ApplyConfig(Config, true);
		HealthComp->InitializeHealth();
		CachedHealth = HealthComp->GetHealth();
	}
	else if (HealthComp)
	{
		CachedHealth = 0.f;
	}
}

void AObstaclesBase::PlayHitFeedback(const FVector& Where)
{
	if (!Data) return;

	if (Data->HitVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, Data->HitVFX, Where, FRotator::ZeroRotator, FVector(Data->VFXScale));
	}
	if (Data->HitSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Data->HitSFX, Where);
	}
}

void AObstaclesBase::PlayDeathFeedback(const FVector& Where)
{
	if (!Data) return;

	const UNiagaraSystem* FX = Data->DeathVFX ? Data->DeathVFX : Data->HitVFX;
	const USoundBase* S = Data->DeathSFX ? Data->DeathSFX : Data->HitSFX;
	const float           Scale = (Data->DeathVFXScale > 0.f) ? Data->DeathVFXScale : Data->VFXScale;

	if (FX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, const_cast<UNiagaraSystem*>(FX), Where, FRotator::ZeroRotator, FVector(Scale));
	}
	if (S)
	{
		UGameplayStatics::PlaySoundAtLocation(this, const_cast<USoundBase*>(S), Where);
	}
}

void AObstaclesBase::ApplyDamage(float Amount, AActor* DamageInstigator)
{
	if (!Data || Amount <= 0.f) return;

	if (DamageInstigator)
	{
		LastDamageInstigator = DamageInstigator;
	}

	if (Data->Health.bHasHealth && HealthComp)
	{
		HealthComp->ApplyDamage(Amount);
		CachedHealth = HealthComp->GetHealth();
		OnDamaged.Broadcast(CachedHealth);
		PlayHitFeedback(GetActorLocation());

		if (Data->Health.bCanDie && CachedHealth <= 0.f)
		{
			// Kill credit to whoever last damaged 
			if (Data->PointsOnKill > 0 && LastDamageInstigator.IsValid())
			{
				AwardPoints(LastDamageInstigator.Get(), Data->PointsOnKill, Data->PointsReason_Kill, this);
			}

			OnDied_Handle();
			OnDied.Broadcast();
		}
	}
	else
	{
		PlayHitFeedback(GetActorLocation());
	}
}

void AObstaclesBase::OnDied_Handle()
{
	if (!Data) { Destroy(); return; }

	PlayDeathFeedback(GetActorLocation());

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

void AObstaclesBase::OnMeshHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	const FHitResult& Hit)
{
	// Feedback first (visual/audio)
	PlayHitFeedback(Hit.ImpactPoint);

	// Optional impact damage
	if (Data && Data->Health.bHasHealth && Data->Health.ImpactDamageScale > 0.f)
	{
		const float Damage = NormalImpulse.Size() * Data->Health.ImpactDamageScale;
		if (Damage > 0.f)
		{
			ApplyDamage(Damage, OtherActor);
		}
	}

	// ---points-on-hit throttling & threshold ---
	if (!Data || Data->PointsOnHit <= 0 || !OtherActor || OtherActor == this)
	{
		return; // nothing to award
	}

	const float Cooldown = Data ? Data->PointsOnHitCooldownSeconds : 0.5f;
	const float MinImpulse = Data ? Data->MinImpulseForHitPoints : 0.f;

	if (MinImpulse > 0.f && NormalImpulse.Size() < MinImpulse)
	{
		return;
	}

	double* LastTimePtr = LastHitScoreTime.Find(OtherActor);
	const double LastTime = LastTimePtr ? *LastTimePtr : -1.0;
	const double Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;

	if (LastTime < 0.0 || (Now - LastTime) >= Cooldown)
	{
		LastHitScoreTime.Add(OtherActor, Now);
		AwardPoints(OtherActor, Data->PointsOnHit, Data->PointsReason_Hit, this);
	}

}

void AObstaclesBase::OnHealthChanged(float NewHealth, float Delta)
{
	CachedHealth = NewHealth;
	OnDamaged.Broadcast(CachedHealth);
}

void AObstaclesBase::OnDied_FromHealth()
{
	
	if (Data && Data->PointsOnKill > 0 && LastDamageInstigator.IsValid())
	{
		AwardPoints(LastDamageInstigator.Get(), Data->PointsOnKill, Data->PointsReason_Kill, this);
	}

	OnDied_Handle();
	OnDied.Broadcast();
}

/** --------- Scoring helper --------- */
void AObstaclesBase::AwardPoints(AActor* Receiver, int32 Amount, FName Reason, AActor* Causer)
{
	if (!Receiver || Amount == 0) return;

	// Interface first
	if (Receiver->GetClass()->ImplementsInterface(UBPI_ScoreReceiver::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Obstacle] AwardPoints via Interface -> %s : +%d (%s)"),
			*GetNameSafe(Receiver), Amount, *Reason.ToString());
		IBPI_ScoreReceiver::Execute_GivePoints(Receiver, Amount, Reason, Causer);
		return;
	}

	// Component fallback
	if (UAC_PointsComponent* PC = Receiver->FindComponentByClass<UAC_PointsComponent>())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Obstacle] AwardPoints via PointsComponent -> %s : +%d (%s)"),
			*GetNameSafe(Receiver), Amount, *Reason.ToString());
		PC->AddPoints(Amount, Reason, Causer);
		return;
	}

	
}

