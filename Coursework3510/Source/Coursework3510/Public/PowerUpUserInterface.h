// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
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
	// Multiply or add to speed. If bMultiply=true, multiply current multiplier by Multiplier; else add amount. 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PowerUps|Speed")
	void ApplySpeedMulti(float Multiplier, bool bMultiply);

	/** Heal by Amount (forward to your health component). */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PowerUps|Health")
	void Heal(float Amount);

	/** Add an upward impulse for jump */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PowerUps|Movement")
	void DoJumpAmount(float Impulse);

	// Base/contact damage scalar used by “Strength”-type effects. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PowerUps|Combat")
	float GetRamDamage() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PowerUps|Combat")
	void SetRamDamage(float Value);

	// Percent for UI. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PowerUps|Health")
	float GetHealthPercent() const;
};