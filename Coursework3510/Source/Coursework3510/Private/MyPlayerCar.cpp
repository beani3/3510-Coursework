// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerCar.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void AMyPlayerCar::BeginPlay() {
	Super::BeginPlay();

	/* Input mapping context 
	 */
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController
				->GetLocalPlayer())) {
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AMyPlayerCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	/* Setting up action bindings
	 */
	if (UEnhancedInputComponent* EnhancedInputComponent =
		CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPlayerCar::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AMyPlayerCar::MoveEnd);

		// Handbreak
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Triggered, this, &AMyPlayerCar::OnHandbrakePressed);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &AMyPlayerCar::OnHandbrakeReleased);
	}
}

void AMyPlayerCar::Move(const FInputActionValue& Value) {
	/* Input is a 2D vector
	 */
	FVector2D MovementVector = Value.Get<FVector2D>();
	GetVehicleMovementComponent()->SetThrottleInput(MovementVector.Y);

	if (MovementVector.Y < 0) {
		GetVehicleMovementComponent()->SetBrakeInput(MovementVector.Y * -1);
	}

	GetVehicleMovementComponent()->SetSteeringInput(MovementVector.X);
}

void AMyPlayerCar::MoveEnd() {
	GetVehicleMovementComponent()->SetBrakeInput(0);
	GetVehicleMovementComponent()->SetThrottleInput(0);
	GetVehicleMovementComponent()->SetSteeringInput(0);
}

void AMyPlayerCar::OnHandbrakePressed() {
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void AMyPlayerCar::OnHandbrakeReleased() {
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

float AMyPlayerCar::CalcSpeed() {
	FVector CurrentVelocity = GetVelocity();
	float CurrentSpeed = CurrentVelocity.Length();
	float ForwardSpeed = FVector::DotProduct(GetVelocity(), GetActorForwardVector());

	return ForwardSpeed;
}