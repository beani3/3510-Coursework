// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "MyAIController.generated.h"

class AMyPlayerCar;

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	AMyPlayerCar* AICar;

	// AI Car getters and setters
	void SetThrottle(float throttleVal);
	float GetThrottleVal();

	void SetSteering(float steeringVal);
	float GetSteeringVal();

	void SetBraking(float brakingVal);
	float GetBrakingVal();

private:
	float ThrottleVal;
	float SteeringVal;
	float BrakingVal;

protected:

};
