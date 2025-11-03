// Fill out your copyright notice in the Description page of Project Settings.

#include "AC_ProjectileComponent.h"
#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Pawn.h"

UAC_ProjectileComponent::UAC_ProjectileComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAC_ProjectileComponent::BeginPlay()
{
	Super::BeginPlay();

	
	if (!ProjectileClass) // Default to AProjectile if none set
	{
		ProjectileClass = AProjectile::StaticClass();
	}
}

USceneComponent* UAC_ProjectileComponent::ResolveMuzzle() const // Resolve the muzzle component
{
	if (MuzzleComponent)
		return MuzzleComponent;

	if (AActor* Owner = GetOwner())
	{
		// Try a component literally named "Muzzle"
		if (USceneComponent* ByName = Cast<USceneComponent>(Owner->GetDefaultSubobjectByName(TEXT("Muzzle"))))
			return ByName; 

		// Try to find any ArrowComponent (optional)
		if (USceneComponent* Arrow = Cast<USceneComponent>(Owner->FindComponentByClass<USceneComponent>()))
			return Arrow;

		// Fallback: root
		return Owner->GetRootComponent();
	}
	return nullptr;
}

FTransform UAC_ProjectileComponent::BuildSpawnTM() const
{
	if (USceneComponent* Muzzle = ResolveMuzzle())
	{
		return Muzzle->GetComponentTransform();
	}
	// Final fallback: push in front of owner using offset
	return MuzzleOffset * GetOwner()->GetActorTransform();
}

bool UAC_ProjectileComponent::FireByDef(const UProjectileDef* Def, USceneComponent* HomingTarget)
{
	if (!GetOwner() || !ProjectileClass || !Def)
		return false;

	
	USceneComponent* Muzzle = ResolveMuzzle();
	APawn* InstPawn = Cast<APawn>(GetOwner());

	const FTransform SpawnTM = Muzzle ? Muzzle->GetComponentTransform()
		: (MuzzleOffset * GetOwner()->GetActorTransform());

	AProjectile* Proj = GetWorld()->SpawnActorDeferred<AProjectile>(
		ProjectileClass, SpawnTM, GetOwner(), InstPawn,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	if (!Proj)
		return false;

	Proj->InitFromDef(Def, GetOwner(), HomingTarget);

	// Ensure correct rotation
	Proj->SetActorTransform(SpawnTM);

	UGameplayStatics::FinishSpawningActor(Proj, SpawnTM);

	UE_LOG(LogTemp, Log, TEXT("[ProjComp] Fired %s from %s | Fwd=%s"),
		*GetNameSafe(Proj),
		*SpawnTM.GetLocation().ToString(),
		*SpawnTM.GetRotation().GetForwardVector().ToString());

	return true;
}