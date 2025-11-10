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

	for (int i = 0; i < NumMetadataPoints; ++i)
	{
		FPathMetadata metadata;

		float pointOneDistance = pointDistance * i;
		float pointTwoDistance = pointDistance * (i+1);

		FVector point1 = SplineComponent->GetLocationAtDistanceAlongSpline(pointOneDistance, ESplineCoordinateSpace::World);
		FVector point2 = SplineComponent->GetLocationAtDistanceAlongSpline(pointTwoDistance, ESplineCoordinateSpace::World);
		DrawDebugSphere(GetWorld(), point1, 15, 10, FColor::Green, false, 15.f);

		FVector tangent1 = SplineComponent->GetTangentAtDistanceAlongSpline(pointOneDistance, ESplineCoordinateSpace::World);
		FVector tangent2 = SplineComponent->GetTangentAtDistanceAlongSpline(pointTwoDistance, ESplineCoordinateSpace::World);
		FVector tangentDelta = tangent2 - tangent1;
		float Curvature = tangentDelta.Size() / pointDistance;
		metadata.CurvatureNormalised = FMath::Clamp(Curvature / MaxExpectedCurvature, 0.f, 1.f);
		metadata.TargetThrottle = 1.f - metadata.CurvatureNormalised;

		SplineMetadata.Add(metadata);
		SplineMetadataLocations.Add(pointOneDistance);
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
