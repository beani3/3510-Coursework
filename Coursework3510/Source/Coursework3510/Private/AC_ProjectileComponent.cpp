// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_ProjectileComponent.h"
#include "Projectile.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UAC_ProjectileComponent::UAC_ProjectileComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

bool UAC_ProjectileComponent::FireByRow(const FProjectileRow& Row, USceneComponent* HomingTarget)
{
	if (!GetOwner() || !ProjectileClass) return false;

	const FTransform SpawnTM = MuzzleOffset * GetOwner()->GetActorTransform();

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());

	AProjectile* Proj = GetWorld()->SpawnActorDeferred<AProjectile>(
		ProjectileClass,                   // TSubclassOf<AProjectile>
		SpawnTM,                           // const FTransform&
		GetOwner(),                        // AActor* Owner
		InstigatorPawn,                    // APawn* Instigator  << IMPORTANT
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
	);

	if (!Proj) return false;

	Proj->InitFromItemRow(Row, GetOwner(), HomingTarget);
	UGameplayStatics::FinishSpawningActor(Proj, SpawnTM);

	return true;
}
// Called when the game starts
void UAC_ProjectileComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!ProjectileClass)
	{
		ProjectileClass = AProjectile::StaticClass();
	}
}
	



// Called every frame
void UAC_ProjectileComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

