
// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemTypes.generated.h"

/* ---- BUFFS (Health only for now) ---- */
UENUM(BlueprintType)
enum class EBuffType : uint8
{
	Health UMETA(DisplayName = "Health")
};

/* ---- PROJECTILES ---- */
UENUM(BlueprintType)
enum class EProjBehavior : uint8
{
	Bouncy UMETA(DisplayName = "Bouncy"),
	Homing UMETA(DisplayName = "Homing")
};

UENUM(BlueprintType)
enum class EProjectileType : uint8
{
	GreenShell UMETA(DisplayName = "Green Shell"),
	RedShell   UMETA(DisplayName = "Red Shell")
};

/* ---- Buff row (DT_Buffs) ---- */
USTRUCT(BlueprintType)
struct FBuffRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Name = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EBuffType BuffType = EBuffType::Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0")) float HealAmount = 50.f;

	/* UI */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSoftObjectPtr<UTexture2D> Icon;
};

/* ---- Projectile row (DT_Projectiles) ---- */
USTRUCT(BlueprintType)
struct FProjectileRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Name = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EProjectileType ProjectileType = EProjectileType::GreenShell;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EProjBehavior Behavior = EProjBehavior::Bouncy;

	/* Motion */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0")) float Speed = 2400.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0")) float LifeSeconds = 6.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0")) int32 MaxBounces = 6;

	/* Bounce filter */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FName> AllowedBounceTags; // empty = allow all
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FName> NoBounceTags;      // no bounce if any match

	/* Visuals */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSoftObjectPtr<UStaticMesh> Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSoftObjectPtr<UMaterialInterface> MeshMaterial;

	/* UI */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSoftObjectPtr<UTexture2D> Icon;
};
