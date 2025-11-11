//

#include "AC_BulletTime.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"

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

	// By tag "Racetrack"
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

	// By name "Racetrack" (actor or component)
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
	if (!Pawn || !Pawn->IsLocallyControlled()) return;

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
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

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

void UAC_BulletTime::ApplyOwnerVisibility(bool bVisible)
{
	if (!OwnerMesh)
	{
		OwnerMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	}
	if (OwnerMesh)
	{
		OwnerMesh->SetVisibility(bVisible, true);
	}
}

void UAC_BulletTime::SpawnOrDestroyVisual(bool bSpawn)
{
	if (bSpawn)
	{
		if (!BulletVisualClass || BulletVisualActor) return;

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
	else
	{
		if (BulletVisualActor)
		{
			BulletVisualActor->Destroy();
			BulletVisualActor = nullptr;
		}
	}
}

// -------- Public API (client-safe) --------

void UAC_BulletTime::StartBulletTime(float DurationSeconds)
{
	if (DurationSeconds <= 0.f) return;

	// Clients request the server to start; server calls multicast
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		ServerStartBulletTime(DurationSeconds);
		return;
	}

	ServerStartBulletTime(DurationSeconds); // server can call directly too
}

void UAC_BulletTime::StopBulletTime()
{
	// Clients ask server to stop; server multicasts stop
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		ServerStopBulletTime();
		return;
	}

	ServerStopBulletTime();
}

// -------- Server RPCs --------

void UAC_BulletTime::ServerStartBulletTime_Implementation(float DurationSeconds)
{
	if (bActive) return;

	// Ensure spline (server)
	if (!RaceSpline)
	{
		RaceSpline = FindRaceSpline();
		if (!RaceSpline)
		{
			UE_LOG(LogTemp, Error, TEXT("[BulletTime] Cannot start: 'Racetrack' spline not found (server)."));
			return;
		}
	}

	const FVector MyLoc = GetOwner()->GetActorLocation();
	const float Key = RaceSpline->FindInputKeyClosestToWorldLocation(MyLoc);
	const float StartDist = RaceSpline->GetDistanceAlongSplineAtSplineInputKey(Key);

	// Arm server timer to stop
	GetWorld()->GetTimerManager().ClearTimer(BulletTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(
		BulletTimerHandle,
		this,
		&UAC_BulletTime::ServerStopBulletTime,
		DurationSeconds,
		false
	);

	// Multicast to all: initialize local state/visuals and (only on server) motion authority
	MulticastStartBulletTime(DurationSeconds, StartDist);
}

void UAC_BulletTime::ServerStopBulletTime_Implementation()
{
	if (!bActive) return;

	GetWorld()->GetTimerManager().ClearTimer(BulletTimerHandle);
	MulticastStopBulletTime();
}

// -------- Multicast RPCs --------

void UAC_BulletTime::MulticastStartBulletTime_Implementation(float DurationSeconds, float StartDistanceOnSpline)
{
	// Ensure spline on each machine
	if (!RaceSpline)
	{
		RaceSpline = FindRaceSpline();
	}

	Duration = DurationSeconds;
	Elapsed = 0.f;
	EndTimeSeconds = GetWorld() ? (GetWorld()->GetTimeSeconds() + static_cast<double>(DurationSeconds)) : 0.0;

	StartDistance = StartDistanceOnSpline;
	CurrentDistance = StartDistanceOnSpline;

	bActive = true;

	// Visuals everywhere
	OwnerMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	ApplyOwnerVisibility(false);
	SpawnOrDestroyVisual(true);

	// Input ignore only on locally controlled pawn
	SetInputIgnored(true);

	// Enable tick on all, but only the server will move the actor
	SetComponentTickEnabled(true);

	UE_LOG(LogTemp, Log, TEXT("[BulletTime] START dur=%.2fs, speed=%.0f, startDist=%.1f"),
		DurationSeconds, SplineSpeed, StartDistance);
}

void UAC_BulletTime::MulticastStopBulletTime_Implementation()
{
	bActive = false;

	// Visuals everywhere
	SpawnOrDestroyVisual(false);
	ApplyOwnerVisibility(true);

	// Input restore only on locally controlled pawn
	SetInputIgnored(false);

	// Tick off everywhere
	SetComponentTickEnabled(false);

	Duration = 0.f;
	Elapsed = 0.f;
	EndTimeSeconds = 0.0;

	UE_LOG(LogTemp, Log, TEXT("[BulletTime] STOP"));
}

// -------- Tick (server-authoritative motion) --------

void UAC_BulletTime::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Only server moves the actor; position/rotation replicate to clients
	if (!bActive || !RaceSpline || !GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	Elapsed += DeltaTime;
	CurrentDistance += SplineSpeed * DeltaTime;

	const float SplineLen = RaceSpline->GetSplineLength();
	const float Dist = FMath::Clamp(CurrentDistance, 0.f, SplineLen);

	const FVector Loc = RaceSpline->GetLocationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
	const FRotator Rot = RaceSpline->GetRotationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
	const FQuat    Q = bOrientToSpline ? Rot.Quaternion() : GetOwner()->GetActorQuat();

	GetOwner()->SetActorLocationAndRotation(Loc, Q, false, nullptr, ETeleportType::TeleportPhysics);

	ZeroPhysicsVelocities(); // keep physics settled on server
}
