// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_HealthComponent.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UAC_HealthComponent::UAC_HealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAC_HealthComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeHealth();

	// ...
	
}

void UAC_HealthComponent::InitializeHealth()
{
	Health = MaxHealth;
	bIsDead = false;
	OnHealthChanged.Broadcast(Health, 0.f);
}

void UAC_HealthComponent::ApplyDamage(float DamageAmount)
{
	if (bIsDead || DamageAmount <= 0.f)
		return;

	const float OldHealth = Health;
	Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);
	const float HealthChange = Health - OldHealth; // Negative if damaged

	TimeSinceLastDamage = 0.f;
	OnDamaged.Broadcast(DamageAmount);
	OnHealthChanged.Broadcast(Health, HealthChange);

	if (Health <= 0.f && bCanDie && !bIsDead)
	{
		bIsDead = true;
		OnDied.Broadcast();
	}
}

void UAC_HealthComponent::Heal(float HealAmount)
{
	if (bIsDead || HealAmount <= 0.f)
		return;

	const float OldHealth = Health;
	Health = FMath::Clamp(Health + HealAmount, 0.f, MaxHealth);
	const float HealthChange = Health - OldHealth; // Positive if healed

	if (HealthChange > 0.f)
	{
		OnHealed.Broadcast(HealthChange);
		OnHealthChanged.Broadcast(Health, HealthChange);
	}
}

void UAC_HealthComponent::SetMaxHealth(float NewMax, bool bClampCurrent)
{
	MaxHealth = FMath::Max(1.f, NewMax);
	if (bClampCurrent)
	{
		Health = FMath::Clamp(Health, 0.f, MaxHealth);
		OnHealthChanged.Broadcast(Health, 0.f);
	}
}

float UAC_HealthComponent::GetHealthPercent() const
{
	return (MaxHealth > 0.f) ? (Health / MaxHealth) : 0.f;
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

