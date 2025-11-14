// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PowerItemDef.h"
#include "BuffDef.generated.h"

// Types of buffs available
UENUM(BlueprintType)
enum class EBuffType : uint8
{
    Heal        UMETA(DisplayName = "Heal"),
    Shield      UMETA(DisplayName = "Shield"),
    SpeedBoost  UMETA(DisplayName = "Speed Boost"),
    DamageBoost UMETA(DisplayName = "Damage Boost")
};

// Definition of a buff power-up item
UCLASS(BlueprintType)
class COURSEWORK3510_API UBuffDef : public UPowerItemDef
{
	GENERATED_BODY()
public:
	// Constructor to set the kind to Buff
    UBuffDef() { Kind = EPowerItemKind::Buff; }

	// kind of buffs
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EBuffType BuffType = EBuffType::Heal;


    

	// magnitude of the buff effect
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Magnitude = 50.f;
};
