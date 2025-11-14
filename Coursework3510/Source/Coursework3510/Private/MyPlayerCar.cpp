// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerCar.h"

#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnrealMathUtility.h"

#include "Components/SplineComponent.h"
#include "Net/UnrealNetwork.h"


#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "ChaosVehicleMovementComponent.h"
#include "Components/ArrowComponent.h"

#include "AC_HealthComponent.h"
#include "AC_PowerupComponentC.h"
#include "AC_ProjectileComponent.h"
#include "AC_PointsComponent.h"

#include "Checkpoints.h"


#include "PS_PlayerState.h"
#include "PC_RaceController.h"

#include "GM_RaceManager.h"
#include "GS_RaceState.h"
#include "ProjectileDef.h"

DEFINE_LOG_CATEGORY_STATIC(LogCarReset, Log, All);

AMyPlayerCar::AMyPlayerCar()
{
	// Create components
	AC_Health = CreateDefaultSubobject<UAC_HealthComponent>(TEXT("HealthComponent"));
	AC_PowerupComponentC = CreateDefaultSubobject<UAC_PowerupComponentC>(TEXT("PowerupComponent"));
	AC_Projectile = CreateDefaultSubobject<UAC_ProjectileComponent>(TEXT("ProjectileComponent"));
	AC_Points = CreateDefaultSubobject<UAC_PointsComponent>(TEXT("PointsComponent"));

	Muzzle = CreateDefaultSubobject<UArrowComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(GetRootComponent()); 
	Muzzle->SetRelativeLocation(FVector(100.f, 0.f, 50.f));

	bReplicates = true;
}

void AMyPlayerCar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyPlayerCar, DistanceOnSpline);
	DOREPLIFETIME(AMyPlayerCar, LapProgress01);
	DOREPLIFETIME(AMyPlayerCar, RacePosition);
}


void AMyPlayerCar::BeginPlay() {
	Super::BeginPlay();

	InitRacetrackSpline();

	if (AC_Health) { AC_Health->InitializeHealth(); }
	if (AC_Projectile && Muzzle) { AC_Projectile->SetMuzzle(Muzzle); }

	if (AGS_RaceState* RS = GetWorld() ? GetWorld()->GetGameState<AGS_RaceState>() : nullptr)
	{
		RS->OnRaceStarted.AddDynamic(this, &AMyPlayerCar::OnRaceStarted);
		RS->OnRaceFinished.AddDynamic(this, &AMyPlayerCar::OnRaceFinished);

		if (RS->bRaceRunning && !RS->bRaceFinished)
		{
			OnRaceStarted();
		}
		else if (RS->bRaceFinished)
		{
			OnRaceFinished();
		}
	}

	// Input mapping context 
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController
				->GetLocalPlayer())) {
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		};
	};
	CurrentLapStartWorldTime = 0.f;
	bHasStartedLapTiming = false;

}


void AMyPlayerCar::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority() && RaceSpline)
	{
		const FVector P = GetActorLocation();

		DistanceOnSpline = GetDistanceAlongTrackAt(P);
		LapProgress01 = GetLapProgress01At(P);

		// Wrong-way detection: compare forward vs spline tangent
		const float Key = RaceSpline->FindInputKeyClosestToWorldLocation(P);
		const FVector Tangent = RaceSpline->GetDirectionAtSplineInputKey(Key, ESplineCoordinateSpace::World);
		const float ForwardDot = FVector::DotProduct(GetActorForwardVector(), Tangent);
		if (ForwardDot < -0.3f) { WrongWayTimer += DeltaSeconds; }
		else { WrongWayTimer = 0.f; }

		// Flip detection
		const float UpDot = FVector::DotProduct(GetActorUpVector(), FVector::UpVector);
		if (UpDot < 0.5f) { FlipTimer += DeltaSeconds; }
		else { FlipTimer = 0.f; }

		// Off-track cheating: distance away from nearest spline point
		const FVector ClosestLoc = RaceSpline->GetLocationAtSplineInputKey(Key, ESplineCoordinateSpace::World);
		const float OffTrack = FVector::Dist2D(ClosestLoc, P);

		const bool bWrongWay = WrongWayTimer >= WrongWaySecondsToReset;
		const bool bFlipped = FlipTimer >= FlipSecondsToReset;
		const bool bCheating = OffTrack >= MaxOffTrackMeters;

		if (bWrongWay || bFlipped || bCheating)
		{
			EResetCause Cause = EResetCause::None;
			if (bWrongWay) { Cause = EResetCause::WrongWay; }
			else if (bFlipped) { Cause = EResetCause::Flipped; }
			else { Cause = EResetCause::OffTrack; }

			LogResetReason(
				Cause,
				ForwardDot, WrongWayTimer, WrongWaySecondsToReset,
				UpDot, FlipTimer, FlipSecondsToReset,
				OffTrack, MaxOffTrackMeters
			);

			ResetToCheckpoint();
			WrongWayTimer = FlipTimer = 0.f;
		}
	}

	if (SpeedSampleInterval > 0.f && MaxSpeedForNormalization > KINDA_SMALL_NUMBER)
	{
		TimeSinceLastSpeedSample += DeltaSeconds;

		if (TimeSinceLastSpeedSample >= SpeedSampleInterval)
		{
			TimeSinceLastSpeedSample = 0.f;

			const float RawSpeedCmPerSec =
				GetVehicleMovementComponent()
				? GetVehicleMovementComponent()->GetForwardSpeed()
				: 0.f;

			const float Normalized =
				FMath::Clamp(FMath::Abs(RawSpeedCmPerSec) / MaxSpeedForNormalization, 0.f, 1.f);

			SmoothedSpeed01 = Normalized;
		}
	}
}




void AMyPlayerCar::InitRacetrackSpline()
{
	if (RaceSpline) return;

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsWithTag(this, RacetrackActorTag, Found);
	for (AActor* A : Found)
	{
		if (!IsValid(A)) continue;
		
		if (USplineComponent* S = Cast<USplineComponent>(A->GetDefaultSubobjectByName(SplineComponentName)))
		{
			RaceSpline = S;
			break;
		}
		
		TArray<USplineComponent*> Splines;
		A->GetComponents(Splines);
		if (Splines.Num() > 0)
		{
			RaceSpline = Splines[0];
			break;
		}
	}
}

float AMyPlayerCar::GetDistanceAlongTrackAt(const FVector& WorldLoc) const
{
	if (!RaceSpline) return 0.f;
	const float Key = RaceSpline->FindInputKeyClosestToWorldLocation(WorldLoc);
	return RaceSpline->GetDistanceAlongSplineAtSplineInputKey(Key);
}



float AMyPlayerCar::GetLapProgress01At(const FVector& WorldLoc) const
{
	if (!RaceSpline) return 0.f;
	const float len = RaceSpline->GetSplineLength();
	if (len <= KINDA_SMALL_NUMBER) return 0.f;
	return GetDistanceAlongTrackAt(WorldLoc) / len;
}


float AMyPlayerCar::GetNormalizedSpeed() const
{
	return SmoothedSpeed01;
}



void AMyPlayerCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	// Setting up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent =
		CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		// Throttle
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &AMyPlayerCar::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &AMyPlayerCar::ThrottleEnd);

		// Brake
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &AMyPlayerCar::Brake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &AMyPlayerCar::BrakeEnd);

		//// Moving
		//EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPlayerCar::Move);
		//EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AMyPlayerCar::MoveEnd);

		// Steering
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &AMyPlayerCar::Steering);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &AMyPlayerCar::SteeringEnd);

		// Handbreak
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Triggered, this, &AMyPlayerCar::OnHandbrakePressed);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &AMyPlayerCar::OnHandbrakeReleased);

		// Pause Menu
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, this, &AMyPlayerCar::Pause);

		// Powerup
		EnhancedInputComponent->BindAction(PowerupAction, ETriggerEvent::Triggered, this, &AMyPlayerCar::UsePowerup);
		
		//Reset
		EnhancedInputComponent->BindAction(ResetAction, ETriggerEvent::Triggered, this, &AMyPlayerCar::ResetToCheckpoint);

	}
}

void AMyPlayerCar::Throttle(const FInputActionValue& Value) {
	const float ThrottleAxis = FMath::Clamp(Value.Get<float>(), 0, 1.f);
	GetVehicleMovementComponent()->SetThrottleInput(ThrottleAxis);
}
void AMyPlayerCar::ThrottleEnd() {
	GetVehicleMovementComponent()->SetThrottleInput(0.f);
}

void AMyPlayerCar::Brake(const FInputActionValue& Value) {
	const float BrakeAxis = FMath::Clamp(Value.Get<float>(), 0, 1.f);
	GetVehicleMovementComponent()->SetBrakeInput(BrakeAxis);
	bIsBraking = true;
}

void AMyPlayerCar::BrakeEnd() {
	GetVehicleMovementComponent()->SetBrakeInput(0.f);
	bIsBraking = false;
}

//void AMyPlayerCar::Move(const FInputActionValue& Value) {
//	// Input is a 2D vector
//	FVector2D MovementVector = Value.Get<FVector2D>();
//	GetVehicleMovementComponent()->SetThrottleInput(MovementVector.Y);
//
//	if (MovementVector.Y < 0) {
//		GetVehicleMovementComponent()->SetBrakeInput(MovementVector.Y * -1);
//	}
//}



//void AMyPlayerCar::MoveEnd() {
//	GetVehicleMovementComponent()->SetBrakeInput(0);
//	GetVehicleMovementComponent()->SetThrottleInput(0);
//}

void AMyPlayerCar::Steering(const FInputActionValue& Value)
{
	const float SteeringAxis = Value.Get<float>();
	GetVehicleMovementComponent()->SetSteeringInput(SteeringAxis);
}

void AMyPlayerCar::SteeringEnd()
{
	GetVehicleMovementComponent()->SetSteeringInput(0.f);
}

void AMyPlayerCar::OnHandbrakePressed() {
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
	bIsBraking = true;
}

void AMyPlayerCar::OnHandbrakeReleased() {
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
	bIsBraking = false;
}

//void AMyPlayerCar::OnPauseEnter() {
//	PauseMenuInst = CreateWidget<UUserWidget>(GetWorld(), PauseMenu);
//	APlayerController* PlayerController = Cast<APlayerController>(Controller);
//
//	PauseMenuInst->AddToViewport();
//	bInPauseMenu = true;
//
//	if (bInPauseMenu) {
//		PlayerController->bShowMouseCursor = true;
//		PlayerController->SetInputMode(FInputModeUIOnly());
//	}
//}


void AMyPlayerCar::Pause()
{
	if (APC_RaceController* RC = Cast<APC_RaceController>(GetController()))
	{
		const bool bPaused = UGameplayStatics::IsGamePaused(GetWorld());
		RC->RequestSetPaused(!bPaused);
	}
}

void AMyPlayerCar::LapCheckpoint(int32 _CheckpointNumber, int32 _MaxCheckpoints, bool _bStartFinishLine)
{
	UE_LOG(LogTemp, Warning, TEXT("LapCheckpoint called!"));

	const float Now = GetWorld()->GetTimeSeconds();

	// Passing the start/finish line
	const bool bCrossingStartFinish =
		(CurrentCheckpoint >= _MaxCheckpoints && _bStartFinishLine);

	if (bCrossingStartFinish)
	{
		if (!bHasStartedLapTiming)
		{
			bHasStartedLapTiming = true;
			CurrentLapStartWorldTime = Now;
		}
		else
		{
			const float LapTime = Now - CurrentLapStartWorldTime;
			CurrentLapStartWorldTime = Now;

			if (AController* PC = Cast<AController>(GetController()))
			{
				if (APS_PlayerState* PS = PC->GetPlayerState<APS_PlayerState>())
				{
					PS->RegisterLapTime(LapTime);
					PS->CurrentLap = Lap + 1;
					PS->CurrentCheckpoint = 1;
				}
			}
		}
		Lap += 1;
		CurrentCheckpoint = 1;
	}
	else if (_CheckpointNumber == CurrentCheckpoint + 1)
	{
		CurrentCheckpoint += 1;
	}
	else if (_CheckpointNumber < CurrentCheckpoint)
	{
		CurrentCheckpoint = _CheckpointNumber;
	}

	UE_LOG(LogTemp, Warning, TEXT("Lap: %i, Checkpoint: %i"), Lap, CurrentCheckpoint);

	if (HasAuthority())
	{
		AGS_RaceState* RS = GetWorld() ? GetWorld()->GetGameState<AGS_RaceState>() : nullptr;
		if (RS && bHasStartedLapTiming)
		{
			const int32 TotalLaps = RS->TotalLaps; 			
			if (Lap > TotalLaps)
			{
				AController* PC = Cast<AController>(GetController());
				APS_PlayerState* PS = PC ? PC->GetPlayerState<APS_PlayerState>() : nullptr;

				if (PS)
				{
					const double NowServer = RS->GetServerWorldTimeSeconds();
					PS->bHasFinished = true;
					PS->FinishTimeSeconds = NowServer - RS->RaceStartServerTime;
				}

				if (APC_RaceController* RacePC = Cast<APC_RaceController>(PC))
				{
					RacePC->ClientShowImmediateWinScreen();
				}

				if (AGM_RaceManager* GM = GetWorld()->GetAuthGameMode<AGM_RaceManager>())
				{
					GM->NotifyPlayerFinished(PS);
				}
			}

		}
	}
}



void AMyPlayerCar::ResetToCheckpoint()
{
	if (!RaceSpline) { InitRacetrackSpline(); if (!RaceSpline) return; }

	AActor* TargetCP = nullptr;
	TArray<AActor*> AllCPs;
	UGameplayStatics::GetAllActorsOfClass(this, ACheckpoints::StaticClass(), AllCPs);
	for (AActor* A : AllCPs)
	{
		if (ACheckpoints* CP = Cast<ACheckpoints>(A))
		{
			const int32 Num = CP->GetClass()->FindPropertyByName(TEXT("CheckpointNumber")) ? CP->CheckpointNumber : -1; // or add accessor on ACheckpoints
			if (Num == CurrentCheckpoint) { TargetCP = CP; break; }
		}
	}
	if (!TargetCP && AllCPs.Num() > 0) TargetCP = AllCPs[0]; 

	const float Key = RaceSpline->FindInputKeyClosestToWorldLocation(TargetCP ? TargetCP->GetActorLocation() : GetActorLocation());
	float Dist = RaceSpline->GetDistanceAlongSplineAtSplineInputKey(Key) + FMath::Max(0.f, RespawnForwardMeters);

	FVector NewLoc = RaceSpline->GetLocationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
	FRotator NewRot = RaceSpline->GetRotationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);

	if (!FMath::IsNearlyZero(RespawnLateralOffset))
	{
		const FVector Right = NewRot.RotateVector(FVector::RightVector);
		NewLoc += Right * RespawnLateralOffset;
	}

	SetActorLocationAndRotation(NewLoc, NewRot, false, nullptr, ETeleportType::TeleportPhysics);
	if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		if (Prim->IsSimulatingPhysics())
		{
			Prim->SetPhysicsLinearVelocity(FVector::ZeroVector);
			Prim->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		}
	}
}

void AMyPlayerCar::LogResetReason(
	EResetCause Cause,
	const float ForwardDot, const float WrongWayTimerVal, const float WrongWayThreshold,
	const float UpDot, const float FlipTimerVal, const float FlipThreshold,
	const float OffTrackDist, const float OffTrackLimit)
{
	const double Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;

	if (LastResetLogTime >= 0.0 && FMath::IsNearlyEqual((float)Now, (float)LastResetLogTime, 0.0001f))
	{
		return;
	}
	LastResetLogTime = Now;

	ResetCount++;
	LastResetCause = Cause;

	const TCHAR* CauseStr =
		(Cause == EResetCause::WrongWay) ? TEXT("WrongWay") :
		(Cause == EResetCause::Flipped) ? TEXT("Flipped") :
		(Cause == EResetCause::OffTrack) ? TEXT("OffTrack") : TEXT("Unknown");

	UE_LOG(LogCarReset, Warning,
		TEXT("[Reset #%d] Cause=%s | ForwardDot=%.2f (Timer=%.2fs/%.2fs) | UpDot=%.2f (Timer=%.2fs/%.2fs) | OffTrack=%.1f/%.1f | Pos=%s"),
		ResetCount,
		CauseStr,
		ForwardDot, WrongWayTimerVal, WrongWayThreshold,
		UpDot, FlipTimerVal, FlipThreshold,
		OffTrackDist, OffTrackLimit,
		*GetActorLocation().ToString()
	);

	if (bShowResetToast)
	{
		const bool bLocallyControlled =
			(IsPlayerControlled() && Cast<APlayerController>(Controller) && Cast<APlayerController>(Controller)->IsLocalController());

		if (bLocallyControlled)
		{
			FString Toast = FString::Printf(
				TEXT("RESET: %s\nFwdDot=%.2f  (%.1fs/%.1fs)\nUpDot=%.2f  (%.1fs/%.1fs)\nOffTrack=%.1fm / %.1fm"),
				CauseStr,
				ForwardDot, WrongWayTimerVal, WrongWayThreshold,
				UpDot, FlipTimerVal, FlipThreshold,
				OffTrackDist, OffTrackLimit
			);

			UKismetSystemLibrary::PrintString(
				this, Toast, true, true,
				FLinearColor(1.f, 0.3f, 0.1f, 1.f), 2.5f
			);
		}
	}
}

void AMyPlayerCar::UsePowerup()
{
	if (AC_PowerupComponentC)
	{
		
		AC_PowerupComponentC->ActivateHeld();
	}
}

void AMyPlayerCar::OnRaceStarted()
{
	// Print Hello string
	UKismetSystemLibrary::PrintString(this, TEXT("Hello"), true, true, FLinearColor(0.0f, 0.66f, 1.0f, 1.0f), 2.0f);

	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void AMyPlayerCar::OnRaceFinished()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

FRaceData AMyPlayerCar::GetRaceData() const
{
	FRaceData D;
	D.RacePosition = RacePosition;
	D.Lap = Lap;
	D.CurrentCheckpoint = CurrentCheckpoint;
	D.LapProgress01 = LapProgress01;     
	return D;
}
