// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AC_HealthComponent.generated.h"


//     Delegate definitions (BlueprintAssignable) 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewHealth, float, HealthChange);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamaged, float, DamageAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealed, float, HealAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDied);

USTRUCT(BlueprintType)
struct FHealthComponentConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxHealth = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool  bCanDie = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool  bAutoRegen = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RegenPerSecond = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RegenDelay = 3.f;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COURSEWORK3510_API UAC_HealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_HealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//  Health state

	//Maximum health value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.f;

	//Current health value
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health(float OldHealth);


	// Can Die?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool bCanDie = true;

	// Is Dead?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	bool bIsDead = false;

	// Auto Regen
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regen")
	bool bAutoRegen = false;

	// Regen per seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regen")
	float RegenPerSecond = 5.f;

	// Regen delay after taking damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regen")
	float RegenDelay = 3.f;

	// Time since last damage taken
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regen")
	float TimeSinceLastDamage = 0.f;

public:	

	// --- Events ---
	// Health changed event
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChanged OnHealthChanged;

	// Damaged event
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDamaged OnDamaged;

	// Healed event
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealed OnHealed;

	// Died event
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDied OnDied;

	// Initializes health to max health
	UFUNCTION(BlueprintCallable, Category = "Health")
	void InitializeHealth();

	// Apply damage to the health component
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyDamage(float DamageAmount);

	// Heal the health component
	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float HealAmount);

	// Set Max Health
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetMaxHealth(float NewMax, bool bClampCurrent = true);

	//  Getters (Pure Blueprint nodes) 

	// Get health percentage (0.0 - 1.0)
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const;

	// Get current health value
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const { return Health; }

	// Get maximum health value
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }

	// Check if the actor is alive
	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsAlive() const { return !bIsDead; }

	// Apply a configuration to the health component
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyConfig(const FHealthComponentConfig& InCfg, bool bResetHealth = true);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
};
