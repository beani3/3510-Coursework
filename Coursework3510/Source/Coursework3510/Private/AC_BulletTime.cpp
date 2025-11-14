// Bullet time is an actor component that, when activated, moves its owner along a predefined spline at high speed,


// Fixed issue with camera jitter on client have casue issues with physics on land needs sorting //

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

	RaceSpline = FindRaceSpline(); // find Spline with tag or name "Racetrack"
	if (!RaceSpline)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BulletTime] BeginPlay: no 'Racetrack' spline found yet.")); // 
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
		{
			if (!A) continue;
			if (USplineComponent* SC = A->FindComponentByClass<USplineComponent>())
				return SC;
		}
	}

	// 2) By name
	{
		TArray<AActor*> All;
		UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), All);
		for (AActor* A : All)
		{
			if (!A) continue;

			if (A->GetName().Equals(TEXT("Racetrack"), ESearchCase::IgnoreCase))
			{
				if (USplineComponent* SC = A->FindComponentByClass<USplineComponent>())
					return SC;
			}

			for (UActorComponent* C : A->GetComponents())
			{
				if (auto* SC = Cast<USplineComponent>(C))
				{
					if (SC->GetName().Equals(TEXT("Racetrack"), ESearchCase::IgnoreCase))
						return SC;
				}
			}
		}
	}

	return nullptr;
}

void UAC_BulletTime::SetInputIgnored(bool bIgnore)  // im not sure this fixes what i was trying but documetntation is so lacking  still able to turn on break lights 
{
	APawn* Pawn = GetOwner() ? Cast<APawn>(GetOwner()) : nullptr;
	if (!Pawn || !Pawn->IsLocallyControlled()) return;

	AController* C = Pawn->GetController();
	if (!C) return;

	if (bIgnore)
	{
		
		bPrevIgnoreMove = C->IsMoveInputIgnored();
		C->SetIgnoreMoveInput(true);
	}
	else
	{
		C->SetIgnoreMoveInput(bPrevIgnoreMove);
	}
}

void UAC_BulletTime::ZeroPhysicsVelocities() const
{
	// No longerneeded but scared to remove in case of the same bug with the duplicated powerup component
}

void UAC_BulletTime::ApplyOwnerVisibility(bool bVisible) //makes the mesh invisible during bullet time
{
	if (!OwnerMesh)
		OwnerMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	if (OwnerMesh)
		OwnerMesh->SetVisibility(bVisible, true);
}

void UAC_BulletTime::SpawnOrDestroyVisual(bool bSpawn) // spawns the bullet time visual replacement
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

void UAC_BulletTime::BoostNetRate(bool bBoost) // increases network update frequency during bullet time to reduce jitter
{
	if (AActor* Owner = GetOwner())
	{
		Owner->SetReplicateMovement(true);

		if (bBoost)
		{
			Owner->SetNetUpdateFrequency(60.f);
			Owner->SetMinNetUpdateFrequency(30.f);
			Owner->NetPriority = 3.f;
		}
		else
		{
			
			Owner->SetNetUpdateFrequency(20.f);
			Owner->SetMinNetUpdateFrequency(2.f); // think this is same as default
			Owner->NetPriority = 1.f;
		}
	}
}


// 

void UAC_BulletTime::StartBulletTime(float DurationSeconds) // starts bullet time for a given duration
{
	if (DurationSeconds <= 0.f) return;

	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		ServerStartBulletTime(DurationSeconds);
		return;
	}
	ServerStartBulletTime(DurationSeconds);
}

void UAC_BulletTime::StopBulletTime() // stops bullet time
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		ServerStopBulletTime();
		return;
	}
	ServerStopBulletTime();
}

// 

void UAC_BulletTime::ServerStartBulletTime_Implementation(float DurationSeconds) // server authoritative start
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

	const FVector MyLoc = GetOwner()->GetActorLocation(); // get actor location
	const float Key = RaceSpline->FindInputKeyClosestToWorldLocation(MyLoc); // find closest point on spline
	const float StartDist = RaceSpline->GetDistanceAlongSplineAtSplineInputKey(Key); // get distance along spline

	// Stop timer in case
	GetWorld()->GetTimerManager().ClearTimer(BulletTimerHandle); // clear any existing timer
	GetWorld()->GetTimerManager().SetTimer( // set timer to stop bullet time after duration
		BulletTimerHandle,
		this,
		&UAC_BulletTime::ServerStopBulletTime,
		DurationSeconds,
		false
	);

	MulticastStartBulletTime(DurationSeconds, StartDist); // multicast to all clients 
}

void UAC_BulletTime::ServerStopBulletTime_Implementation() // server authoritative stop
{
	if (!bActive) return;

	GetWorld()->GetTimerManager().ClearTimer(BulletTimerHandle);
	MulticastStopBulletTime();
}

// 

void UAC_BulletTime::MulticastStartBulletTime_Implementation(float DurationSeconds, float StartDistanceOnSpline) // multicast start 
{
	if (!RaceSpline)
		RaceSpline = FindRaceSpline();

	Duration = DurationSeconds;
	Elapsed = 0.f;
	LocalElapsed = 0.f;
	EndTimeSeconds = GetWorld() ? (GetWorld()->GetTimeSeconds() + (double)DurationSeconds) : 0.0;

	StartDistance = StartDistanceOnSpline;
	CurrentDistance = StartDistanceOnSpline;

	bActive = true;

	OwnerMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	ApplyOwnerVisibility(false);
	SpawnOrDestroyVisual(true);

	SetInputIgnored(true);
	SetComponentTickEnabled(true);

	BoostNetRate(true);
}

void UAC_BulletTime::MulticastStopBulletTime_Implementation() // multicast stop
{
	bActive = false;

	SpawnOrDestroyVisual(false);
	ApplyOwnerVisibility(true);

	SetInputIgnored(false);
	SetComponentTickEnabled(false);

	Duration = 0.f;
	Elapsed = 0.f;
	LocalElapsed = 0.f;
	EndTimeSeconds = 0.0;

	BoostNetRate(false);
}

void UAC_BulletTime::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bActive || !RaceSpline || !GetOwner())
		return;

	AActor* Owner = GetOwner();
	const bool bIsAuthority = Owner->HasAuthority();

	APawn* Pawn = Cast<APawn>(Owner);
	const bool bIsLocallyControlled = Pawn && Pawn->IsLocallyControlled();


	if (bIsAuthority)
	{
		Elapsed += DeltaTime;
		CurrentDistance = StartDistance + SplineSpeed * Elapsed;

		const float SplineLen = RaceSpline->GetSplineLength();
		const float Dist = FMath::Clamp(CurrentDistance, 0.f, SplineLen);

		const FVector  Loc = RaceSpline->GetLocationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		const FRotator Rot = RaceSpline->GetRotationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		const FQuat    Q = bOrientToSpline ? Rot.Quaternion() : Owner->GetActorQuat();

		Owner->SetActorLocationAndRotation(Loc, Q, false, nullptr, ETeleportType::TeleportPhysics);

		
		ZeroPhysicsVelocities();
	}
	
	else if (bClientVisualSmoothing && bIsLocallyControlled)
	{
		LocalElapsed += DeltaTime;
		const float PredictedDist = StartDistance + SplineSpeed * LocalElapsed;

		const float SplineLen = RaceSpline->GetSplineLength();
		const float Dist = FMath::Clamp(PredictedDist, 0.f, SplineLen);

		const FVector  Loc = RaceSpline->GetLocationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		const FRotator Rot = RaceSpline->GetRotationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		const FQuat    Q = bOrientToSpline ? Rot.Quaternion() : Owner->GetActorQuat();

		
		Owner->SetActorLocationAndRotation(Loc, Q, false, nullptr, ETeleportType::TeleportPhysics);
	}
	
}
