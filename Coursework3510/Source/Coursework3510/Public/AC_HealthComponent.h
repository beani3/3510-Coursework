// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AC_HealthComponent.generated.h"


// --- Delegate definitions (BlueprintAssignable) ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewHealth, float, HealthChange);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamaged, float, DamageAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealed, float, HealAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDied);

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
	// --- Health state ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool bCanDie = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	bool bIsDead = false;

	// --- Auto Regen ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regen")
	bool bAutoRegen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regen")
	float RegenPerSecond = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regen")
	float RegenDelay = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regen")
	float TimeSinceLastDamage = 0.f;

public:	

	// --- Delegates ---
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDamaged OnDamaged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealed OnHealed;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDied OnDied;

	// --- Core API ---
	UFUNCTION(BlueprintCallable, Category = "Health")
	void InitializeHealth();

	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float HealAmount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetMaxHealth(float NewMax, bool bClampCurrent = true);

	// --- Getters (Pure Blueprint nodes) ---
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsAlive() const { return !bIsDead; }

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
