// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BuffDef.h"
#include "ProjectileDef.h"
#include "PowerUpUserInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UPowerUpUserInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COURSEWORK3510_API IPowerUpUserInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
public:
    // Buffs 
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PowerUps|Buffs")
    void ApplyBuff(const UBuffDef* Buff);

    // Projectiles 
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PowerUps|Projectiles")
    void FireProjectileFromDef(const UProjectileDef* Def);
};
