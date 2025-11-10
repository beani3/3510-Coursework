// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "MyAIController.generated.h"

class AMyPlayerCar;
class USplineComponent;
class AMyAIPath;

UCLASS()
class COURSEWORK3510_API AMyAIController : public AAIController
{
	GENERATED_BODY()	
public:
	void BeginPlay();
	void Tick(float DeltaTime);

	/* Having the player car as a pointer to the 
	 * AI car means we can access the neccessary variables
	 * to make the AI move.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	AMyPlayerCar* AICar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	AMyAIPath* CurrentSpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float LookAheadDistance = 1500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MaxSpeed = 4000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float ThrottleGain = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float BrakeGain = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<AMyAIPath*> AIPathArray;
		
	// AI Car getters 
	// void CalcThrottle();
	float GetThrottleVal();

	float CalcSteering();
	float GetSteeringVal();

	// void CalcBreaking();
	float GetBrakingVal();

	FVector FindClosestLocationAlongAIPath(FVector point, float offset = 0.f);
	void SetCurrentSpline(int8 SplineIndex);

private:
	float ThrottleVal;
	float SteeringVal;
	float BrakingVal;

protected:
};
