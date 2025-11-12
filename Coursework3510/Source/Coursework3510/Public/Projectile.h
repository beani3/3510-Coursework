// Projectile.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
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

	// Server-only: configure from data asset + shooter
	void InitFromDef(const UProjectileDef* Def, AActor* InInstigator, USceneComponent* HomingTarget);

	// Server-only: set replicated asset path so clients load same visuals
	void SetDefPathFromDef(const UProjectileDef* Def);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USphereComponent* Collision = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UStaticMeshComponent* Mesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UProjectileMovementComponent* Move = nullptr;

	// Definition + shooter
	UPROPERTY() const UProjectileDef* Data = nullptr;
	UPROPERTY() AActor* InstigatorActor = nullptr;

	// Replicated def path for client visuals
	UPROPERTY(ReplicatedUsing = OnRep_DefPath) FSoftObjectPath DefPath;
	UFUNCTION() void OnRep_DefPath();

	// Visuals/lifespan
	void ApplyVisualsFromDef(const UProjectileDef* Def);
	void ApplyLifespanFromDef(const UProjectileDef* Def);

	// Events
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* Other, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& Sweep);

	

	// Helpers
	bool ShouldBounceOff(const AActor* Other) const;
	bool IsValidVictim(AActor* Other) const;
	
	void Die();

	void DoImpactOnValidVictim(AActor* Victim, const FVector& Where);



private:
	// Ignore shooter for a short time after spawn
	double IgnoreShooterUntilTime = 0.0;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Tuning")
	float ArmingDelaySeconds = 0.12f;

	int32 BounceCount = 0;
};
