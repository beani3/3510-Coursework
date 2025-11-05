// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerCar.h"

#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "ChaosVehicleMovementComponent.h"
#include "Components/ArrowComponent.h"

#include "AC_HealthComponent.h"
#include "AC_PowerupComponentC.h"
#include "AC_ProjectileComponent.h"
#include "AC_PointsComponent.h"



#include "GM_RaceManager.h"
#include "ProjectileDef.h"

AMyPlayerCar::AMyPlayerCar()
{
	// Create components
	AC_Health = CreateDefaultSubobject<UAC_HealthComponent>(TEXT("HealthComponent"));
	AC_PowerupComponentC = CreateDefaultSubobject<UAC_PowerupComponentC>(TEXT("PowerupComponent"));
	AC_Projectile = CreateDefaultSubobject<UAC_ProjectileComponent>(TEXT("ProjectileComponent"));
	AC_Points = CreateDefaultSubobject<UAC_PointsComponent>(TEXT("PointsComponent"));

	//Add Interface


	Muzzle = CreateDefaultSubobject<UArrowComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(GetRootComponent()); // AWheeledVehiclePawn already has a root
	Muzzle->SetRelativeLocation(FVector(100.f, 0.f, 50.f)); // tweak in editor as needed


}



void AMyPlayerCar::BeginPlay() {
	Super::BeginPlay();


	// Initialize Health
	if (AC_Health)
	{
		AC_Health->InitializeHealth();
	}

	// Set Muzzle on Projectile Component
	if (AC_Projectile && Muzzle)
	{
		AC_Projectile->SetMuzzle(Muzzle);
	}





	// Get Game Mode and cast to GM_RaceManager to bind race events
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(this);
	if (GameMode)
	{
		AGM_RaceManager* RaceManager = Cast<AGM_RaceManager>(GameMode);
		if (RaceManager)
		{
			GMRaceRef = RaceManager;
			// Bind OnStarted event
			RaceManager->OnStarted.AddDynamic(this, &AMyPlayerCar::OnRaceStarted);
			// Bind OnFinished event
			RaceManager->OnFinished.AddDynamic(this, &AMyPlayerCar::OnRaceFinished);
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
}

void AMyPlayerCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	// Setting up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent =
		CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPlayerCar::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AMyPlayerCar::MoveEnd);

		// Steering
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &AMyPlayerCar::Steering);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &AMyPlayerCar::SteeringEnd);

		// Handbreak
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Triggered, this, &AMyPlayerCar::OnHandbrakePressed);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &AMyPlayerCar::OnHandbrakeReleased);

		// Pause Menu
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, this, &AMyPlayerCar::OnPauseEnter);

		// powerup
		EnhancedInputComponent->BindAction(PowerupAction, ETriggerEvent::Triggered, this, &AMyPlayerCar::UsePowerup);

	}
}

void AMyPlayerCar::Move(const FInputActionValue& Value) {
	// Input is a 2D vector
	FVector2D MovementVector = Value.Get<FVector2D>();
	GetVehicleMovementComponent()->SetThrottleInput(MovementVector.Y);

	if (MovementVector.Y < 0) {
		GetVehicleMovementComponent()->SetBrakeInput(MovementVector.Y * -1);
	}
}



void AMyPlayerCar::MoveEnd() {
	GetVehicleMovementComponent()->SetBrakeInput(0);
	GetVehicleMovementComponent()->SetThrottleInput(0);
}

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
}

void AMyPlayerCar::OnHandbrakeReleased() {
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void AMyPlayerCar::OnPauseEnter() {
	PauseMenuInst = CreateWidget<UUserWidget>(GetWorld(), PauseMenu);
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	
	PauseMenuInst->AddToViewport();
	bInPauseMenu = true;
	
	if (bInPauseMenu) {
		PlayerController->bShowMouseCursor = true;
		PlayerController->SetInputMode(FInputModeUIOnly());
	}
}

void AMyPlayerCar::LapCheckpoint(int32 _CheckpointNumber, int32 _MaxCheckpoints, bool _bStartFinishLine) 
{
	UE_LOG(LogTemp, Warning, TEXT("LapCheckpoint called!"));
	
	if (CurrentCheckpoint >= _MaxCheckpoints && _bStartFinishLine == true)
	{
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
}


// Input Event: When key 'X' is pressed
void AMyPlayerCar::UsePowerup()
{
	if (AC_PowerupComponentC)
	{
		// Activate held powerup
		AC_PowerupComponentC->ActivateHeld();
	}
}




// Race Started event handler
void AMyPlayerCar::OnRaceStarted()
{
	// Print Hello string
	UKismetSystemLibrary::PrintString(this, TEXT("Hello"), true, true, FLinearColor(0.0f, 0.66f, 1.0f, 1.0f), 2.0f);

	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

// Race Finished event handler
void AMyPlayerCar::OnRaceFinished()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}