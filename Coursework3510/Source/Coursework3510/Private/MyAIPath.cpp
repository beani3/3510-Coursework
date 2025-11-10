// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAIPath.h"
#include "Components/SplineComponent.h"

// Sets default values
AMyAIPath::AMyAIPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("AI Path"));
}

// Called when the game starts or when spawned
void AMyAIPath::BeginPlay()
{
	Super::BeginPlay();

	float splineLength = SplineComponent->GetSplineLength();
	float pointDistance = splineLength / NumMetadataPoints;


	bool inCorner = false;
	float highestCurvature = 0.f;
	int apexIndex = 0;
	for (int i = 0; i < NumMetadataPoints; ++i)
	{
		FPathMetadata metadata;

		float pointSplineDistance = pointDistance * i;
		float pointBehindDistance = pointSplineDistance - pointDistance * 0.5f;
		float pointAheadDistance = pointSplineDistance + pointDistance * 0.5f;

		FVector point = SplineComponent->GetLocationAtDistanceAlongSpline(pointSplineDistance, ESplineCoordinateSpace::World);
		DrawDebugSphere(GetWorld(), point, 15, 10, FColor::Green, false, 15.f);

		FVector tangent0 = SplineComponent->GetTangentAtDistanceAlongSpline(pointBehindDistance, ESplineCoordinateSpace::World);
		FVector tangent1 = SplineComponent->GetTangentAtDistanceAlongSpline(pointSplineDistance, ESplineCoordinateSpace::World);
		FVector tangent2 = SplineComponent->GetTangentAtDistanceAlongSpline(pointAheadDistance, ESplineCoordinateSpace::World);
		tangent0.Normalize();
		tangent1.Normalize();
		tangent2.Normalize();
		FVector tangentDeltaA = tangent1 - tangent0;
		FVector tangentDeltaB = tangent2 - tangent1;
		float Curvature = (tangentDeltaA.Size() + tangentDeltaB.Size()) / pointDistance;
		metadata.CurvatureNormalised = FMath::Clamp(Curvature / MaxExpectedCurvature, 0.f, 1.f);
		metadata.TargetSpeed = FMath::Lerp(MaxStraightSpeed, MinCornerSpeed, metadata.CurvatureNormalised);

		SplineMetadata.Add(metadata);
		SplineMetadataLocations.Add(pointSplineDistance);
	}
}

// Called every frame
void AMyAIPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Simple binary search that returns the next data point after given distance
FPathMetadata AMyAIPath::GetMetadataAtDistance(float distance)
{
	int Low = 0;
	int High = SplineMetadataLocations.Num() - 1;

	while (Low < High)
	{
		int Mid = (Low + High) / 2;
		if (distance < SplineMetadataLocations[Mid])
			High = Mid;
		else
			Low = Mid + 1;
	}

	// High is the first index where SplineMetadataLocations[High] > distance
	return SplineMetadata[High];
}
