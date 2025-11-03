// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PowerItemDef.h"
#include "ProjectileDef.generated.h"

// Projectile behavior types
UENUM(BlueprintType)
enum class EProjBehavior : uint8 { Bouncy, Homing };

// Definition for projectile power items
UCLASS(BlueprintType)
class COURSEWORK3510_API UProjectileDef : public UPowerItemDef
{
	GENERATED_BODY()

public:
	// Set kind to Projectile
	UProjectileDef() { Kind = EPowerItemKind::Projectile; }

	// Behavior type
	UPROPERTY(EditAnywhere, BlueprintReadOnly) EProjBehavior Behavior = EProjBehavior::Bouncy;

	// Projectile Movement
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0")) float Speed = 2400.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0")) float LifeSeconds = 6.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0")) int32 MaxBounces = 6;

	// Bounce filters 
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FName> AllowedBounceTags;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FName> NoBounceTags;

	// Visuals
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<UStaticMesh> Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<UMaterialInterface> MeshMaterial;

	// Sounds (Not Used Yet)
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<USoundBase> FireSFX;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<USoundBase> ImpactSFX;
};
