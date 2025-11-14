// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectileDef.h"
#include "AC_ProjectileComponent.generated.h"

class AProjectile;
class USceneComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COURSEWORK3510_API UAC_ProjectileComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAC_ProjectileComponent();

protected:
    virtual void BeginPlay() override;

public:
	// Projectile class to spawn
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    TSubclassOf<AProjectile> ProjectileClass;
	// Muzzle offset from the component's transform
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    FTransform MuzzleOffset;
	// Muzzle scene component (overrides MuzzleOffset if set)
    UFUNCTION(BlueprintCallable, Category = "Projectile")
    void SetMuzzle(USceneComponent* InMuzzle) { MuzzleComponent = InMuzzle; }
	// Fire a projectile based on the given definition, optionally homing on a target
    UFUNCTION(BlueprintCallable, Category = "Projectile")
    bool FireByDef(const UProjectileDef* Def, USceneComponent* HomingTarget = nullptr);

private:
	// Server RPC to handle firing projectiles
    UFUNCTION(Server, Reliable)
    void ServerFireByDef(const UProjectileDef* Def, USceneComponent* HomingTarget = nullptr);

	// Muzzle component pointer
    UPROPERTY()
    USceneComponent* MuzzleComponent = nullptr;
	// Resolve the muzzle component or use the offset
    USceneComponent* ResolveMuzzle() const;
	// Build the spawn transform for the projectile
    FTransform BuildSpawnTM() const;
	// Play muzzle effects at the given transform   (not implemented) 
    void PlayMuzzleFX(const FTransform& SpawnTM);
    
	// Pick a homing target ahead of the owner
    USceneComponent* PickAheadHomingTarget() const;
};
