// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PowerItemDef.h"
#include "BuffDef.generated.h"

UENUM(BlueprintType)
enum class EBuffType : uint8
{
    Heal        UMETA(DisplayName = "Heal"),
    Shield      UMETA(DisplayName = "Shield"),
    SpeedBoost  UMETA(DisplayName = "Speed Boost"),
    DamageBoost UMETA(DisplayName = "Damage Boost")
};

UCLASS(BlueprintType)
class COURSEWORK3510_API UBuffDef : public UPowerItemDef
{
	GENERATED_BODY()
public:
    UBuffDef() { Kind = EPowerItemKind::Buff; }

    /** What kind of buff this is */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EBuffType BuffType = EBuffType::Heal;

    /** Power of the buff — interpretation depends on BuffType */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Magnitude = 50.f;
};
