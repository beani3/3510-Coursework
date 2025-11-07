// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_BulletTime.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"

UAC_BulletTime::UAC_BulletTime()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAC_BulletTime::BeginPlay()
{
	Super::BeginPlay();
	
	RaceSpline = FindRaceSpline();
	if (!RaceSpline)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BulletTime] Could not find a SplineComponent named 'Racetrack' in level."));
	}
}

USplineComponent* UAC_BulletTime::FindRaceSpline()
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	{
		TArray<AActor*> Tagged;
		UGameplayStatics::GetAllActorsWithTag(World, FName("Racetrack"), Tagged);
		for (AActor* A : Tagged)
		{
			if (A)
			{
				if (USplineComponent* SC = A->FindComponentByClass<USplineComponent>())
				{
					UE_LOG(LogTemp, Log, TEXT("[BulletTime] Found spline via Actor tag on %s"), *A->GetName());
					return SC;
				}
			}
		}
	}
		
	

	UE_LOG(LogTemp, Warning, TEXT("[BulletTime] No Racetrack spline found."));
	return nullptr;
}
void UAC_BulletTime::SetInputIgnored(bool bIgnore)
{
	APawn* Pawn = GetOwner() ? Cast<APawn>(GetOwner()) : nullptr;
	if (!Pawn) return;

	AController* C = Pawn->GetController();
	if (!C) return;

	if (bIgnore)
	{
		bPrevIgnoreMove = C->IsMoveInputIgnored();
		bPrevIgnoreLook = C->IsLookInputIgnored();
		C->SetIgnoreMoveInput(true);
		C->SetIgnoreLookInput(true);
	}
	else
	{
		C->SetIgnoreMoveInput(bPrevIgnoreMove);
		C->SetIgnoreLookInput(bPrevIgnoreLook);
	}
}

void UAC_BulletTime::StartBulletTime(float DurationSeconds)
{
	if (DurationSeconds <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BulletTime] Ignored start: duration <= 0"));
		return;
	}

	if (!RaceSpline)
	{
		RaceSpline = FindRaceSpline();
		if (!RaceSpline)
		{
			UE_LOG(LogTemp, Error, TEXT("[BulletTime] No 'Racetrack' spline found; cannot start."));
			return;
		}
	}

	Duration = DurationSeconds;
	Elapsed = 0.f;
	bActive = true;

	// Take over input
	SetInputIgnored(true);

	//make invulnerable / disable physics if needed
	// GetOwner()->SetActorEnableCollision(false);
}

void UAC_BulletTime::StopBulletTime()
{
	if (!bActive) return;

	bActive = false;
	Elapsed = 0.f;
	Duration = 0.f;

	// Restore input
	SetInputIgnored(false);

	// restore collision
	// GetOwner()->SetActorEnableCollision(true);
}

void UAC_BulletTime::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bActive || !RaceSpline) return;

	Elapsed += DeltaTime;
	const float Alpha = FMath::Clamp(Duration > 0.f ? (Elapsed / Duration) : 1.f, 0.f, 1.f);

	const float Length = RaceSpline->GetSplineLength();
	const float Dist = Alpha * Length;

	const FVector Loc = RaceSpline->GetLocationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
	const FRotator Rot = RaceSpline->GetRotationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);

	const FQuat UseRot = bOrientToSpline ? Rot.Quaternion() : GetOwner()->GetActorQuat();
	GetOwner()->SetActorLocationAndRotation(Loc, UseRot, false, nullptr, ETeleportType::TeleportPhysics);

	if (Alpha >= 1.f)
	{
		StopBulletTime();
	}
}
