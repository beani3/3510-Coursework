#include "AC_BulletTime.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"

UAC_BulletTime::UAC_BulletTime()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetComponentTickEnabled(false); 
}

void UAC_BulletTime::BeginPlay()
{
	Super::BeginPlay();


	RaceSpline = FindRaceSpline();
	if (!RaceSpline)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BulletTime] BeginPlay: no 'Racetrack' spline found yet."));
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
					UE_LOG(LogTemp, Log, TEXT("[BulletTime] Found spline via Actor Tag on '%s'."), *A->GetName());
					return SC;
				}
			}
		}
	}

	
	{
		TArray<AActor*> All;
		UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), All);
		for (AActor* A : All)
		{
			if (!A) continue;

			if (A->GetName().Equals(TEXT("Racetrack"), ESearchCase::IgnoreCase))
			{
				if (USplineComponent* SC = A->FindComponentByClass<USplineComponent>())
				{
					UE_LOG(LogTemp, Log, TEXT("[BulletTime] Found spline on actor named 'Racetrack'."));
					return SC;
				}
			}

			for (UActorComponent* C : A->GetComponents())
			{
				if (auto* SC = Cast<USplineComponent>(C))
				{
					if (SC->GetName().Equals(TEXT("Racetrack"), ESearchCase::IgnoreCase))
					{
						UE_LOG(LogTemp, Log, TEXT("[BulletTime] Found component named 'Racetrack' on '%s'."), *A->GetName());
						return SC;
					}
				}
			}
		}
	}

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

void UAC_BulletTime::ZeroPhysicsVelocities() const
{
	
	if (USkeletalMeshComponent* Skel = GetOwner()->FindComponentByClass<USkeletalMeshComponent>())
	{
		Skel->SetPhysicsLinearVelocity(FVector::ZeroVector);
		Skel->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		return;
	}
	
	if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent()))
	{
		if (Prim->IsSimulatingPhysics())
		{
			Prim->SetPhysicsLinearVelocity(FVector::ZeroVector);
			Prim->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		}
	}
}

void UAC_BulletTime::StartBulletTime(float DurationSeconds)
{
	if (DurationSeconds <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BulletTime] Ignoring Start: duration <= 0"));
		return;
	}

	if (bActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BulletTime] Start ignored: already active."));
		return;
	}

	if (!RaceSpline)
	{
		RaceSpline = FindRaceSpline();
		if (!RaceSpline)
		{
			UE_LOG(LogTemp, Error, TEXT("[BulletTime] Cannot start: 'Racetrack' spline not found."));
			return;
		}
	}

	const FVector MyLoc = GetOwner()->GetActorLocation();
	const float Key = RaceSpline->FindInputKeyClosestToWorldLocation(MyLoc);
	StartDistance = RaceSpline->GetDistanceAlongSplineAtSplineInputKey(Key);
	CurrentDistance = StartDistance;

	
	Duration = DurationSeconds;
	Elapsed = 0.f;
	EndTimeSeconds = GetWorld()->GetTimeSeconds() + static_cast<double>(DurationSeconds);
	bActive = true;

	
	SetComponentTickEnabled(true);

	
	SetInputIgnored(true);
	ZeroPhysicsVelocities();

	
	GetWorld()->GetTimerManager().SetTimer(
		BulletTimerHandle,
		this,
		&UAC_BulletTime::StopBulletTime,
		DurationSeconds,
		false
	);

	OwnerMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	if (OwnerMesh)
	{
		OwnerMesh->SetVisibility(false, true);
	}

	// Spawn the bullet visual
	if (BulletVisualClass)
	{
		const FTransform SpawnTM = GetOwner()->GetActorTransform();
		BulletVisualActor = GetWorld()->SpawnActor<AActor>(BulletVisualClass, SpawnTM);

		if (BulletVisualActor)
		{
			
			BulletVisualActor->AttachToComponent(
				GetOwner()->GetRootComponent(),
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				AttachSocketName
			);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[BulletTime] START dur=%.2fs, speed=%.0f, startDist=%.1f"),
		DurationSeconds, SplineSpeed, StartDistance);
}

void UAC_BulletTime::StopBulletTime()
{
	if (!bActive) return;

	
	GetWorld()->GetTimerManager().ClearTimer(BulletTimerHandle);

	bActive = false;

	if (BulletVisualActor)
	{
		BulletVisualActor->Destroy();
		BulletVisualActor = nullptr;
	}

	
	if (OwnerMesh)
	{
		OwnerMesh->SetVisibility(true, true);
	}



	
	SetInputIgnored(false);

	SetComponentTickEnabled(false);
	Duration = 0.f;
	Elapsed = 0.f;
	EndTimeSeconds = 0.0;

	UE_LOG(LogTemp, Log, TEXT("[BulletTime] STOP"));
}

void UAC_BulletTime::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bActive || !RaceSpline) return;

	// Advance by speed (cm/s)
	Elapsed += DeltaTime;
	CurrentDistance += SplineSpeed * DeltaTime;

	const float SplineLen = RaceSpline->GetSplineLength();
	const float Dist = FMath::Clamp(CurrentDistance, 0.f, SplineLen);

	const FVector Loc = RaceSpline->GetLocationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
	const FRotator Rot = RaceSpline->GetRotationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
	const FQuat    Q = bOrientToSpline ? Rot.Quaternion() : GetOwner()->GetActorQuat();

	
	GetOwner()->SetActorLocationAndRotation(Loc, Q, false, nullptr, ETeleportType::TeleportPhysics);

	
	ZeroPhysicsVelocities();
}
	
