 // Fill out your copyright notice in the Description page of Project Settings.


#include "AC_HealthComponent.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UAC_HealthComponent::UAC_HealthComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;


}

// Called when the game starts
void UAC_HealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Initialize health on begin play
	InitializeHealth();

	// ...
	
}
// Initialize health to max health
void UAC_HealthComponent::InitializeHealth()
{
	Health = MaxHealth;
	bIsDead = false;
	OnHealthChanged.Broadcast(Health, 0.f);
}
// Apply damage to the health component
void UAC_HealthComponent::ApplyDamage(float DamageAmount)
{
	if (bIsDead || DamageAmount <= 0.f)
		return;

	const float OldHealth = Health;// Store old health
	Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);// Reduce health
	const float HealthChange = Health - OldHealth; // Negative if damaged

	TimeSinceLastDamage = 0.f; // Reset regen timer
	OnDamaged.Broadcast(DamageAmount);// Broadcast damage event
	OnHealthChanged.Broadcast(Health, HealthChange);// Broadcast health changed event

	if (Health <= 0.f && bCanDie && !bIsDead)// Check for death 
	{
		bIsDead = true; // Set dead flag
		OnDied.Broadcast();// Broadcast death event
	}
}

void UAC_HealthComponent::Heal(float HealAmount)// Heal the health component
{
	if (bIsDead || HealAmount <= 0.f)// Cannot heal if dead or invalid amount
		return;

	const float OldHealth = Health;// Store old health
	Health = FMath::Clamp(Health + HealAmount, 0.f, MaxHealth);// Increase health
	const float HealthChange = Health - OldHealth; // Positive if healed

	if (HealthChange > 0.f)// Only broadcast if there was an actual Health change
	{
		OnHealed.Broadcast(HealthChange);// Broadcast healed event
		OnHealthChanged.Broadcast(Health, HealthChange);// Broadcast health changed event
	}
}

void UAC_HealthComponent::SetMaxHealth(float NewMax, bool bClampCurrent)// Set maximum health
{
	MaxHealth = FMath::Max(1.f, NewMax);// Ensure max health is at least 1
	if (bClampCurrent)
	{
		Health = FMath::Clamp(Health, 0.f, MaxHealth);// Clamp current health to new max
		OnHealthChanged.Broadcast(Health, 0.f);// Broadcast health changed event with no change
	}
}

float UAC_HealthComponent::GetHealthPercent() const// Get current health as a percentage of max health
{
	return (MaxHealth > 0.f) ? (Health / MaxHealth) : 0.f;// Avoid division by zero
}


// Called every frame
void UAC_HealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bAutoRegen || bIsDead)
		return;

	TimeSinceLastDamage += DeltaTime;

	if (TimeSinceLastDamage >= RegenDelay && Health < MaxHealth)
	{
		Heal(RegenPerSecond * DeltaTime);
	}

	// ...
}

void UAC_HealthComponent::ApplyConfig(const FHealthComponentConfig& InCfg, bool bResetHealth)
{
	MaxHealth = FMath::Max(1.f, InCfg.MaxHealth);
	bCanDie = InCfg.bCanDie;
	bAutoRegen = InCfg.bAutoRegen;
	RegenPerSecond = InCfg.RegenPerSecond;
	RegenDelay = InCfg.RegenDelay;

	if (bResetHealth)
	{
		InitializeHealth(); // sets Health=MaxHealth, clears dead, broadcasts
	}
}

