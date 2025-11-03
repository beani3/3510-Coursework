
// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemTypes.generated.h"

/* ---- BUFFS (Health only for now) ---- */
UENUM(BlueprintType)
enum class EBuffTypes2 : uint8
{
	Health UMETA(DisplayName = "Health")
};

/* ---- PROJECTILES ---- */
UENUM(BlueprintType)
enum class EProjBehaviors2 : uint8
{
	Bouncy UMETA(DisplayName = "Bouncy"),
	Homing UMETA(DisplayName = "Homing")
};

UENUM(BlueprintType)
enum class EProjectileTypes2 : uint8
{
	GreenShell UMETA(DisplayName = "Green Shell"),
	RedShell   UMETA(DisplayName = "Red Shell")
};

/* ---- Buff row (DT_Buffs) ---- */
USTRUCT(BlueprintType)
struct FBuffRow : public FTableRowBase
{
	GENERATED_BODY()

	
};

/* ---- Projectile row (DT_Projectiles) ---- */
USTRUCT(BlueprintType)
struct FProjectileRow : public FTableRowBase
{
	GENERATED_BODY()

	
};
