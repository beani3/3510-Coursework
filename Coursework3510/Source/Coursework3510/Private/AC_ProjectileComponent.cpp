// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_ProjectileComponent.h"
#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

UAC_ProjectileComponent::UAC_ProjectileComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAC_ProjectileComponent::BeginPlay()
{
	Super::BeginPlay();

	// If you forget to assign a class, it’ll still work in-editor if AProjectile is the default
	if (!ProjectileClass)
	{
		ProjectileClass = AProjectile::StaticClass();
	}
}

bool UAC_ProjectileComponent::FireByDef(const UProjectileDef* Def, USceneComponent* HomingTarget)
{
	if (!GetOwner() || !ProjectileClass || !Def)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ProjectileComponent] FireByDef failed — invalid input."));
		return false;
	}

	// Build spawn transform using MuzzleOffset (can be set in Blueprint)
	const FTransform SpawnTM = MuzzleOffset * GetOwner()->GetActorTransform();

	// Safe instigator cast
	APawn* InstPawn = Cast<APawn>(GetOwner());

	// Deferred spawn lets you configure projectile before it fully spawns
	AProjectile* Proj = GetWorld()->SpawnActorDeferred<AProjectile>(
		ProjectileClass,
		SpawnTM,
		GetOwner(),
		InstPawn,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
	);

	if (!Proj)
	{
		UE_LOG(LogTemp, Error, TEXT("[ProjectileComponent] SpawnActorDeferred returned null."));
		return false;
	}

	// Initialize projectile using data from the Data Asset
	Proj->InitFromDef(Def, GetOwner(), HomingTarget);

	// Complete the spawn
	UGameplayStatics::FinishSpawningActor(Proj, SpawnTM);

	return true;
}
