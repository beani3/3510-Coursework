// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "WheeledVehiclePawn.h"
#include "MyPlayerCar.h"

void AMyAIController::BeginPlay() {
	Super::BeginPlay();
	
	AICar = Cast<AMyPlayerCar>(GetPawn());
	AICar->GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void AMyAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	FVector CurrentLocation = AICar->GetActorLocation();

	SetThrottle(1);
}

// Throttle value is between 0 and 1
void AMyAIController::SetThrottle(float throttleVal) {
	AICar->GetVehicleMovementComponent()->SetThrottleInput(throttleVal);
}

float AMyAIController::GetThrottleVal() {
	ThrottleVal = AICar->GetVehicleMovementComponent()->GetThrottleInput();
	return ThrottleVal;
}

// Steering value is between -1 and 1
void AMyAIController::SetSteering(float steeringVal) {
	AICar->GetVehicleMovementComponent()->SetSteeringInput(steeringVal);
}

float AMyAIController::GetSteeringVal() {
	SteeringVal = AICar->GetVehicleMovementComponent()->GetSteeringInput();
	return SteeringVal;
}

// Braking value is between 0 and 1
void AMyAIController::SetBraking(float brakingVal) {
	AICar->GetVehicleMovementComponent()->SetBrakeInput(brakingVal);
}

float AMyAIController::GetBrakingVal() {
	BrakingVal = AICar->GetVehicleMovementComponent()->GetBrakeInput();
	return BrakingVal;
}