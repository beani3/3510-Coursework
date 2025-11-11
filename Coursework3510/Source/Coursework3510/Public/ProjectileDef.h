// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PowerItemDef.h"
#include "ProjectileDef.generated.h"

class AProjectile;
class UStaticMesh;
class UMaterialInterface;
class USoundBase;

UENUM(BlueprintType)
enum class EProjBehavior : uint8 { Bouncy, Homing };

UCLASS(BlueprintType)
class COURSEWORK3510_API UProjectileDef : public UPowerItemDef
{
    GENERATED_BODY()

public:
    UProjectileDef() { Kind = EPowerItemKind::Projectile; }

    // === Spawn class (let the asset choose which projectile to spawn) ===
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
    TSubclassOf<AProjectile> ProjectileClass;

    // Behavior
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behavior")
    EProjBehavior Behavior = EProjBehavior::Bouncy;

    // Movement
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Params", meta = (ClampMin = "0")) float Speed = 2400.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Params", meta = (ClampMin = "0")) float LifeSeconds = 6.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Params", meta = (ClampMin = "0")) int32 MaxBounces = 6;

    // Bounce filters
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bounce") TArray<FName> AllowedBounceTags;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bounce") TArray<FName> NoBounceTags;

    // Visuals
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual") TSoftObjectPtr<UStaticMesh> Mesh;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual") TSoftObjectPtr<UMaterialInterface> MeshMaterial;

    // Sounds (optional)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio") TSoftObjectPtr<USoundBase> FireSFX;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio") TSoftObjectPtr<USoundBase> ImpactSFX;
};
