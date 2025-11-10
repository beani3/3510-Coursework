// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "WheeledVehiclePawn.h"
#include "MyPlayerCar.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MyAIPath.h"

void AMyAIController::BeginPlay() {
	Super::BeginPlay();
	
	AICar = Cast<AMyPlayerCar>(GetPawn());
	
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyAIPath::StaticClass(), OutActors);
	for (AActor* FoundActor : OutActors) {
		AIPathArray.Add(Cast<AMyAIPath>(FoundActor));
	}

	SetCurrentSpline(AICar->CurrentSpline);
}

void AMyAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	AICar->GetVehicleMovementComponent()->SetSteeringInput(CalcSteering());

	float DistAlongSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtLocation(AICar->GetActorLocation(), ESplineCoordinateSpace::World);
	FPathMetadata metadata = CurrentSpline->GetMetadataAtDistance(DistAlongSpline);
	FPathMetadata lookaheadMetadata = CurrentSpline->GetMetadataAtDistance(DistAlongSpline + LookAheadDistance);
	float normalisedSpeed = AICar->GetVehicleMovementComponent()->GetForwardSpeed() / MaxSpeed;
	float normalisedTargetSpeed = metadata.TargetSpeed / MaxSpeed;
	float lookaheadNormalisedTargetSpeed = lookaheadMetadata.TargetSpeed / MaxSpeed;
	float speedError = normalisedTargetSpeed - normalisedSpeed;
	float lookaheadSpeedError = lookaheadNormalisedTargetSpeed - normalisedSpeed;

	float throttle = FMath::Clamp(speedError * ThrottleGain, 0.f, 1.f);
	float brake = FMath::Clamp(-lookaheadSpeedError * BrakeGain, 0.f, 1.f);

	AICar->GetVehicleMovementComponent()->SetThrottleInput(throttle);
	AICar->GetVehicleMovementComponent()->SetBrakeInput(brake);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, FString::SanitizeFloat(metadata.TargetSpeed));
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString::SanitizeFloat(AICar->GetVehicleMovementComponent()->GetForwardSpeed()));
	}
}

// Throttle value is between 0 and 1
float AMyAIController::GetThrottleVal() {
	ThrottleVal = AICar->GetVehicleMovementComponent()->GetThrottleInput();
	return ThrottleVal;
}

float AMyAIController::CalcSteering() {
	// Get closest point to spline
	FVector AILocation = AICar->GetActorLocation();
	FVector LocationToSteerTo = FindClosestLocationAlongAIPath(AILocation, LookAheadDistance);
	// Calculate steering angle
	FRotator AIRotation = AICar->GetActorRotation();
	FRotator SteerRotation = UKismetMathLibrary::FindLookAtRotation(AILocation, LocationToSteerTo);
	FRotator AngleFromPath = UKismetMathLibrary::NormalizedDeltaRotator(SteerRotation, AIRotation);

	// Convert angle into steering input
	return UKismetMathLibrary::MapRangeClamped(AngleFromPath.Yaw, -10.0, 10.0, -1.0, 1.0);
}

// Steering value is between -1 and 1
float AMyAIController::GetSteeringVal() {
	SteeringVal = AICar->GetVehicleMovementComponent()->GetSteeringInput();
	return SteeringVal;
}

// Braking value is between 0 and 1
float AMyAIController::GetBrakingVal() {
	BrakingVal = AICar->GetVehicleMovementComponent()->GetBrakeInput();
	return BrakingVal;
}

FVector AMyAIController::FindClosestLocationAlongAIPath(FVector point, float offset) {
	float DistAlongSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtLocation(point, ESplineCoordinateSpace::World) + offset;
	float splineLength = CurrentSpline->SplineComponent->GetSplineLength();
	if (DistAlongSpline > splineLength)
		DistAlongSpline -= splineLength;

	FVector LocationToFollow = CurrentSpline->SplineComponent->GetLocationAtDistanceAlongSpline(DistAlongSpline, ESplineCoordinateSpace::World);
	return LocationToFollow;
}

void AMyAIController::SetCurrentSpline(int8 SplineIndex) {
	CurrentSpline = AIPathArray[SplineIndex];
}