// Fill out your copyright notice in the Description page of Project Settings.

#include "AC_ProjectileComponent.h"
#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "UObject/SoftObjectPath.h"


UAC_ProjectileComponent::UAC_ProjectileComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UAC_ProjectileComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UAC_ProjectileComponent::FireByDef(const UProjectileDef* Def, USceneComponent* HomingTarget)
{
	if (!Def || !ProjectileClass || !GetOwner() || !GetWorld())
		return false;

	// Client -> ask server to spawn
	if (!GetOwner()->HasAuthority())
	{
		ServerFireByDef(Def, HomingTarget);
		return true;
	}

	// === SERVER ONLY ===
	const FTransform SpawnTM = BuildSpawnTM();

	AProjectile* Proj = GetWorld()->SpawnActorDeferred<AProjectile>(
		ProjectileClass,
		SpawnTM,
		GetOwner(),
		Cast<APawn>(GetOwner()),
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	if (!Proj)
		return false;

	// Provide full init server-side (sets visuals + movement)
	Proj->InitFromDef(Def, GetOwner(), HomingTarget);

	
	UGameplayStatics::FinishSpawningActor(Proj, SpawnTM);

	PlayMuzzleFX(SpawnTM);

	UE_LOG(LogTemp, Log, TEXT("[ProjComp] Fired %s from %s | Fwd=%s"),
		*GetNameSafe(Proj),
		*SpawnTM.GetLocation().ToString(),
		*SpawnTM.GetRotation().GetForwardVector().ToString());

	return true;
}

void UAC_ProjectileComponent::ServerFireByDef_Implementation(const UProjectileDef* Def, USceneComponent* HomingTarget)
{
	FireByDef(Def, HomingTarget);
}

USceneComponent* UAC_ProjectileComponent::ResolveMuzzle() const
{
	// Priority 1: explicit
	if (MuzzleComponent && MuzzleComponent->IsRegistered())
		return MuzzleComponent;

	// Priority 2: named/tagged "Muzzle"
	AActor* Owner = GetOwner();
	if (!Owner) return nullptr;

	for (UActorComponent* C : Owner->GetComponents())
	{
		if (USceneComponent* SC = Cast<USceneComponent>(C))
		{
			if (SC->GetFName() == TEXT("Muzzle") || SC->ComponentHasTag(TEXT("Muzzle")))
				return SC;
		}
	}

	// Priority 3: root
	return Owner->GetRootComponent();
}

FTransform UAC_ProjectileComponent::BuildSpawnTM() const
{
	const USceneComponent* Muzzle = ResolveMuzzle();
	FTransform BaseTM = Muzzle ? Muzzle->GetComponentTransform()
		: (GetOwner() ? GetOwner()->GetActorTransform() : FTransform::Identity);

	// Local offset * parent/world TM
	return MuzzleOffset * BaseTM;
}

void UAC_ProjectileComponent::PlayMuzzleFX(const FTransform& SpawnTM)
{
	// Example:
	// if (MuzzleFX) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFX, SpawnTM);
	// if (MuzzleSound) UGameplayStatics::PlaySoundAtLocation(GetWorld(), MuzzleSound, SpawnTM.GetLocation());
}
