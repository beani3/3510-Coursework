//

#include "AC_BulletTime.h"
#include "Kismet/GameplayStatics.h"
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

	// 1) By tag
	{
		TArray<AActor*> Tagged;
		UGameplayStatics::GetAllActorsWithTag(World, FName("Racetrack"), Tagged);
		for (AActor* A : Tagged)
			if (A)
				if (USplineComponent* SC = A->FindComponentByClass<USplineComponent>())
					return SC;
	}

	// 2) By name
	{
		TArray<AActor*> All;
		UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), All);
		for (AActor* A : All)
		{
			if (!A) continue;

			if (A->GetName().Equals(TEXT("Racetrack"), ESearchCase::IgnoreCase))
				if (USplineComponent* SC = A->FindComponentByClass<USplineComponent>())
					return SC;

			for (UActorComponent* C : A->GetComponents())
				if (auto* SC = Cast<USplineComponent>(C))
					if (SC->GetName().Equals(TEXT("Racetrack"), ESearchCase::IgnoreCase))
						return SC;
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
		OwnerMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	if (OwnerMesh)
		OwnerMesh->SetVisibility(bVisible, true);
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

void UAC_BulletTime::BoostNetRate(bool bBoost)
{
	if (AActor* Owner = GetOwner())
	{
		Owner->SetReplicateMovement(true);

		if (bBoost)
		{
			Owner->SetNetUpdateFrequency(60.f);     // was: Owner->NetUpdateFrequency = 60.f;
			Owner->SetMinNetUpdateFrequency(30.f);  // was: Owner->MinNetUpdateFrequency = 30.f;
			Owner->NetPriority= 3.f;             // was: Owner->NetPriority = 3.f;
		}
		else
		{
			// restore your defaults (tune to your project)
			Owner->SetNetUpdateFrequency(20.f);
			Owner->SetMinNetUpdateFrequency(2.f);
			Owner->NetPriority= 1.f;
		}
	}
}


// ---------- Public API ----------

void UAC_BulletTime::StartBulletTime(float DurationSeconds)
{
	if (DurationSeconds <= 0.f) return;

	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		ServerStartBulletTime(DurationSeconds);
		return;
	}
	ServerStartBulletTime(DurationSeconds);
}

void UAC_BulletTime::StopBulletTime()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		ServerStopBulletTime();
		return;
	}
	ServerStopBulletTime();
}

// ---------- Server RPCs ----------

void UAC_BulletTime::ServerStartBulletTime_Implementation(float DurationSeconds)
{
	if (bActive) return;

	if (!RaceSpline)
	{
		RaceSpline = FindRaceSpline();
		if (!RaceSpline)
		{
			UE_LOG(LogTemp, Error, TEXT("[BulletTime] No 'Racetrack' spline (server)."));
			return;
		}
	}

	const FVector MyLoc = GetOwner()->GetActorLocation();
	const float Key = RaceSpline->FindInputKeyClosestToWorldLocation(MyLoc);
	const float StartDist = RaceSpline->GetDistanceAlongSplineAtSplineInputKey(Key);

	// Arm stop timer (server)
	GetWorld()->GetTimerManager().ClearTimer(BulletTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(BulletTimerHandle, this, &UAC_BulletTime::ServerStopBulletTime, DurationSeconds, false);

	MulticastStartBulletTime(DurationSeconds, StartDist);
}

void UAC_BulletTime::ServerStopBulletTime_Implementation()
{
	if (!bActive) return;

	GetWorld()->GetTimerManager().ClearTimer(BulletTimerHandle);
	MulticastStopBulletTime();
}

// ---------- Multicast ----------

void UAC_BulletTime::MulticastStartBulletTime_Implementation(float DurationSeconds, float StartDistanceOnSpline)
{
	if (!RaceSpline)
		RaceSpline = FindRaceSpline();

	Duration = DurationSeconds;
	Elapsed = 0.f;
	EndTimeSeconds = GetWorld() ? (GetWorld()->GetTimeSeconds() + (double)DurationSeconds) : 0.0;

	StartDistance = StartDistanceOnSpline;
	CurrentDistance = StartDistanceOnSpline;

	bActive = true;

	OwnerMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	ApplyOwnerVisibility(false);
	SpawnOrDestroyVisual(true);

	SetInputIgnored(true);
	SetComponentTickEnabled(true);

	// Cache smoothing start (client only)
	if (!GetOwner()->HasAuthority())
	{
		SmoothedVisualTM = GetOwner()->GetActorTransform();
	}

	// Reduce replication jitter during BT
	BoostNetRate(true);
}

void UAC_BulletTime::MulticastStopBulletTime_Implementation()
{
	bActive = false;

	SpawnOrDestroyVisual(false);
	ApplyOwnerVisibility(true);

	SetInputIgnored(false);
	SetComponentTickEnabled(false);

	Duration = 0.f;
	Elapsed = 0.f;
	EndTimeSeconds = 0.0;

	BoostNetRate(false);
}

// ---------- Tick ----------

void UAC_BulletTime::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bActive || !RaceSpline || !GetOwner())
		return;

	// Authoritative motion
	if (GetOwner()->HasAuthority())
	{
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
	else if (bClientVisualSmoothing)
	{
		// Client: smooth the visual mesh towards replicated actor transform
		const FTransform TargetTM = GetOwner()->GetActorTransform();
		const float Alpha = 1.f - FMath::Exp(-ClientSmoothStrength * DeltaTime);
		SmoothedVisualTM.Blend(SmoothedVisualTM, TargetTM, Alpha);

		if (USkeletalMeshComponent* VisualRoot = GetOwner()->FindComponentByClass<USkeletalMeshComponent>())
		{
			VisualRoot->SetWorldLocationAndRotation(
				SmoothedVisualTM.GetLocation(),
				SmoothedVisualTM.GetRotation()
			);
		}
	}
}
