// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstaclesBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AObstaclesBase::AObstaclesBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create mesh component and make it the root
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create collision box and attach to Mesh
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(Mesh);
	// Default collision profile - adjust to your needs
	Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Collision->SetGenerateOverlapEvents(false);

	// Bind hit handler
	if (Collision)
	{
		Collision->OnComponentHit.AddDynamic(this, &AObstaclesBase::OnCollisionHit);
	}
}

void AObstaclesBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Apply chosen asset to the mesh for editor preview
	if (Mesh && MeshAsset)
	{
		Mesh->SetStaticMesh(MeshAsset);
	}

	// Auto-size collision to mesh bounds if requested
	if (bAutoSizeCollisionToMeshBounds && Collision && Mesh && Mesh->GetStaticMesh())
	{
		const FVector BoxExtent = Mesh->Bounds.BoxExtent;
		Collision->SetBoxExtent(BoxExtent);
		// Ensure collision location matches mesh origin (relative)
		Collision->SetRelativeLocation(FVector::ZeroVector);
	}
}

void AObstaclesBase::BeginPlay()
{
	Super::BeginPlay();

	// Ensure the asset is applied at runtime
	if (Mesh && MeshAsset)
	{
		Mesh->SetStaticMesh(MeshAsset);
	}

	// Runtime auto-size as well
	if (bAutoSizeCollisionToMeshBounds && Collision && Mesh && Mesh->GetStaticMesh())
	{
		const FVector BoxExtent = Mesh->Bounds.BoxExtent;
		Collision->SetBoxExtent(BoxExtent);
		Collision->SetRelativeLocation(FVector::ZeroVector);
	}
}

void AObstaclesBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AObstaclesBase::OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	HandlePostEffectsOnHit(OtherActor, OtherComp, NormalImpulse, Hit);
}

void AObstaclesBase::HandlePostEffectsOnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, const FVector& NormalImpulse, const FHitResult& Hit)
{
	// Prevent multiple FX in a single frame (simple guard)
	if (bFXPlayed)
	{
		return;
	}
	bFXPlayed = true;

	PlayEffectsAtHit(Hit);
}

void AObstaclesBase::PlayEffectsAtHit(const FHitResult& Hit)
{
	UWorld* World = GetWorld();
	if (!World)
		return;

	// Play Niagara VFX if set
	if (HitVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, HitVFX, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), FVector(VFXScale));
	}

	// Play SFX if set
	if (HitSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSFX, Hit.ImpactPoint);
	}
}

