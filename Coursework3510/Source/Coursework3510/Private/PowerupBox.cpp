// PowerupBox.cpp
#include "PowerupBox.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "TimerManager.h"

#include "AC_PowerupComponentC.h"     
#include "PowerItemDef.h"        

APowerupBox::APowerupBox()
{
	PrimaryActorTick.bCanEverTick = false;

	DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(DefaultRoot);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(DefaultRoot);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(DefaultRoot);
	Sphere->InitSphereRadius(SphereRadius);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionObjectType(ECC_WorldDynamic);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Overlap);

	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	RotatingMovement->RotationRate = SpinRate;
	RotatingMovement->bRotationInLocalSpace = bSpinInLocalSpace;
	RotatingMovement->PivotTranslation = SpinPivot;
	RotatingMovement->bUpdateOnlyIfRendered = bSpinOnlyWhenRendered;

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &APowerupBox::OnBeginOverlap);
}

void APowerupBox::BeginPlay()
{
	Super::BeginPlay();
	Sphere->SetSphereRadius(SphereRadius, true);

	if (!CurrentDef)
	{
		RandomizeFromPool();
	}
}

void APowerupBox::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	Sphere->SetSphereRadius(SphereRadius, true);
	ApplyRotationSettings();
}

void APowerupBox::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsConsumed || !OtherActor) return;

	if (UAC_PowerupComponentC* PowerC = OtherActor->FindComponentByClass<UAC_PowerupComponentC>())
	{
		const bool bAlreadyHas = PowerC->HasItem();
		if (!bAlreadyHas)
		{
			const bool bGiven = PowerC->GiveItem(CurrentDef);
			if (bGiven)
			{
				Consume();
			}
		}
	}
}

void APowerupBox::Consume()
{
	if (bIsConsumed) return;
	bIsConsumed = true;

	SetPickupVisible(false);
	SetPickupCollision(false);
	OnConsumedBP();

	if (RespawnTime > 0.f)
	{
		GetWorldTimerManager().SetTimer(
			TimerHandle_Respawn, this, &APowerupBox::Respawn, RespawnTime, false);
	}
}

void APowerupBox::Respawn()
{
	if (!bIsFixed) 
	{
	RandomizeFromPool();
	}
	SetPickupVisible(true);
	SetPickupCollision(true);
	bIsConsumed = false;
	OnRespawnedBP();
}

void APowerupBox::RandomizeFromPool()
{
	if (PickUpPool.Num() > 0)
	{
		const int32 Index = FMath::RandRange(0, PickUpPool.Num() - 1);
		CurrentDef = PickUpPool[Index];
	}
}

void APowerupBox::ApplyRotationSettings() const
{
	if (!RotatingMovement) return;
	RotatingMovement->RotationRate = SpinRate;
	RotatingMovement->bRotationInLocalSpace = bSpinInLocalSpace;
	RotatingMovement->PivotTranslation = SpinPivot;
	RotatingMovement->bUpdateOnlyIfRendered = bSpinOnlyWhenRendered;
}

void APowerupBox::SetPickupVisible(bool bVisible) const
{
	Mesh->SetVisibility(bVisible, true);
	const_cast<APowerupBox*>(this)->SetActorHiddenInGame(!bVisible);
}

void APowerupBox::SetPickupCollision(bool bEnable) const
{
	Sphere->SetCollisionEnabled(bEnable ? ECollisionEnabled::QueryOnly
		: ECollisionEnabled::NoCollision);
}
