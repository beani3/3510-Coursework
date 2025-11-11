// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "UObject/SoftObjectPath.h"         // FSoftObjectPath
#include "ProjectileDef.h"
#include "Projectile.generated.h"

UCLASS()
class COURSEWORK3510_API AProjectile : public AActor
{
    GENERATED_BODY()

public:
    AProjectile();

    // Initialize from definition (server only)
    void InitFromDef(const UProjectileDef* Def, AActor* InInstigator, USceneComponent* HomingTarget);

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // --- Components ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USphereComponent* Collision = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* Mesh = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UProjectileMovementComponent* Move = nullptr;

    // --- Data & replication ---
    // Replicated soft path so clients can load the same Def for visuals/lifespan
    UPROPERTY(ReplicatedUsing = OnRep_DefPath)
    FSoftObjectPath DefPath;

    // Resolved def (not replicated)
    UPROPERTY(Transient)
    const UProjectileDef* Data = nullptr;

    UPROPERTY(Transient)
    AActor* InstigatorActor = nullptr;

    UFUNCTION()
    void OnRep_DefPath();

    // Helpers used by InitFromDef and OnRep_DefPath
    void ApplyVisualsFromDef(const UProjectileDef* Def);
    void ApplyLifespanFromDef(const UProjectileDef* Def);

    // --- Bounce/Hit ---
    int32 BounceCount = 0;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* Other, UPrimitiveComponent* OtherComp,
        FVector NormalImpulse, const FHitResult& Hit);

    UFUNCTION()
    void OnBounce(const FHitResult& Impact, const FVector& Vel);

    bool ShouldBounceOff(const AActor* Other) const;
    void Die();
};
