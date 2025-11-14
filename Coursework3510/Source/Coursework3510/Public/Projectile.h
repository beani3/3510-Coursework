// Projectile.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "ProjectileDef.h"
#include "Projectile.generated.h"

class UAC_HealthComponent;
class UAC_PointsComponent;

UCLASS()
class COURSEWORK3510_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();

	// Server-only: configure from data asset + shooter + optional homing target
	void InitFromDef(const UProjectileDef* Def, AActor* InInstigator, USceneComponent* HomingTarget);

	// Server-only: replicate asset path so clients load same projectile def
	void SetDefPathFromDef(const UProjectileDef* Def);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* Move;

	// Data + shooter
	UPROPERTY()
	const UProjectileDef* Data;

	UPROPERTY()
	AActor* InstigatorActor;

	// Replicated def path for client visuals
	UPROPERTY(ReplicatedUsing = OnRep_DefPath)
	FSoftObjectPath DefPath;

	UFUNCTION()
	void OnRep_DefPath();

	// Hit-only event
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* Other, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

	//is victim valid for impact?
	bool IsValidVictim(AActor* Other) const;
	// Apply visuals properties from def
	void ApplyVisualsFromDef(const UProjectileDef* Def);
	// Apply lifespan from def
	void ApplyLifespanFromDef(const UProjectileDef* Def);
	// Handle impact on valid victim
	void DoImpactOnValidVictim(AActor* Victim, const FVector& Where);
	// Clamp height to spline if needed so projectile follows track
	void ClampHeightToSpline(float DeltaSeconds);
	// Destroy projectile
	void Die();

	USplineComponent* FindRaceSpline() const;

private:
	// Ignore shooter for a short time after spawn
	double IgnoreShooterUntilTime = 0.0;

	// How long after spawn we still ignore the shooter
	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Tuning")
	float ArmingDelaySeconds = 0.12f;

	// How hard homing projectiles can turn 
	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Tuning")
	float HomingAccelerationMagnitude = 30000.f;

	// Offset above the spline
	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Tuning")
	float SplineHeightOffsetZ = 50.f;

	// impact daamage
	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Tuning")
	float ImpactDamage = 25.f;

	// impact points
	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Tuning")
	int32 ImpactPoints = 50;
	// current bounce count
	int32 BounceCount = 0;

	// Track spline
	UPROPERTY()
	USplineComponent* RaceSpline = nullptr;

	// Whether we clamp to spline height
	bool bClampToSplineHeight = false;

	//Custom "Projectile" object channel = 2nd custom channel in project settings
	static constexpr ECollisionChannel ProjectileChannel = ECC_GameTraceChannel2;
};
