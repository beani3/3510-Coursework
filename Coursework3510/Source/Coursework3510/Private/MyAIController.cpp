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
	AICar->GetVehicleMovementComponent()->SetThrottleInput(metadata.TargetThrottle);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, FString::SanitizeFloat(metadata.TargetThrottle));
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