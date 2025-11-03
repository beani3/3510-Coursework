// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerCar.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"

void AMyPlayerCar::BeginPlay() {
	Super::BeginPlay();

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

void AMyPlayerCar::CallCreateLobby()
{
	UWorld* World = GetWorld();
	{
		World->ServerTravel("/Game/Levels/SplineMapTest?listen");
	}
}

void AMyPlayerCar::CallClientTravel(const FString& Address)
{
	APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
	if (PlayerController)
	{
		PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}
}