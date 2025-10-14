// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObstaclesBase.h"
#include "StaticObstacles.generated.h"

/**
 * 
 */
UCLASS()
class COURSEWORK3510_API AStaticObstacles : public AObstaclesBase
{
	GENERATED_BODY()

public:
	AStaticObstacles();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ObstacleGameplay")

	float KnockbackScale;

	virtual void BeginPlay() override;
	virtual void HandleVehicleHit(AActor* OtherActor, const FHitResult& Hit, float OtherSpeed) override;
	
};
