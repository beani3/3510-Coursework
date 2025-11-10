// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyAIPath.generated.h"

class USplineComponent;

USTRUCT()
struct FPathMetadata
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float CurvatureNormalised; // Angle diff from current point to next point

	UPROPERTY(EditAnywhere)
	float TargetThrottle; // Ideal speed at the current point, used to determine throttle/braking values

	UPROPERTY(EditAnywhere)
	bool BrakeStart; // Flags brake zone start, used to slow down before tight corners

	UPROPERTY(EditAnywhere)
	bool IsApex; // Flags corner apex, used to end braking
};


UCLASS()
class COURSEWORK3510_API AMyAIPath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyAIPath();

	UPROPERTY(EditAnywhere, Category = "Spline")
	USplineComponent* SplineComponent;

	UPROPERTY(EditAnywhere, Category = "Spline")
	FVector SplineOffset;

	UPROPERTY(EditAnywhere, Category = "Spline")
	int NumMetadataPoints = 50;

	UPROPERTY(EditAnywhere, Category = "Spline")
	float ThrottleCurvatureFactor = 50.f;

	UPROPERTY(EditAnywhere, Category = "Spline")
	float MaxExpectedCurvature = 5.f;

	UPROPERTY(EditAnywhere, Category = "Spline")
	TArray<FPathMetadata> SplineMetadata;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FPathMetadata GetMetadataAtDistance(float distance);

private:
	TArray<float> SplineMetadataLocations;
};
